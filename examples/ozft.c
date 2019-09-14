// A program to help do fuzz testing of the Ozunreduce and Unimplode6a
// libraries, particularly with afl-fuzz.
// Written by Jason Summers, 2019.
//
// This utility reads a ZIP file from stdin, decompresses the member file
// at offset 0, and writes the decompressed data to stdout.
//
// This is not a proper ZIP file parser. It only does just enough for
// our purposes.
//
// Typical usage:
//
//   make ozft-afl
//   mkdir -p testcase_dir findings_dir
//   [Copy some small test files into testcase_dir.]
//   afl-fuzz -i testcase_dir -o findings_dir -- ./ozft-afl

/*
========================== TERMS OF USE for ozft.c ===========================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
==============================================================================
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "unimplode6a.h"
#include "ozunreduce.h"

#define FT_MAX_INPUT_SIZE    2000000

struct ft_ctx {
	size_t inbuf_pos;
	size_t inbuf_size;
	size_t nbytes_emitted;
	unsigned char inbuf[FT_MAX_INPUT_SIZE];
};

// Read from our copy of the file data.
static size_t ft_read(struct ft_ctx *ctx, unsigned char *buf, size_t size)
{
	if((ctx->inbuf_pos >= ctx->inbuf_size) ||
		(ctx->inbuf_pos + size > ctx->inbuf_size))
	{
		memset(buf, 0, size);
		return 0;
	}
	memcpy(buf, &ctx->inbuf[ctx->inbuf_pos], size);
	ctx->inbuf_pos += size;
	return size;
}

// Process the decompressed data.
static size_t ft_write(struct ft_ctx *ctx, const unsigned char *buf, size_t size)
{
	// We could do this:
	//fwrite(buf, 1, size, stdout);

	ctx->nbytes_emitted += size;
	return size;
}

static void ft_seek_set(struct ft_ctx *ctx, off_t newpos)
{
	ctx->inbuf_pos = (size_t)newpos;
}

static void skipbytes(struct ft_ctx *ctx, off_t n)
{
	ctx->inbuf_pos += (size_t)n;
}

static uint32_t readui32(struct ft_ctx *ctx)
{
	unsigned char buf[4] = {0};

	ft_read(ctx, buf, 4);
	return (uint32_t)buf[0] | ((uint32_t)buf[1]<<8) |
		((uint32_t)buf[2]<<16) | ((uint32_t)buf[3]<<24);
}

static uint32_t readui16(struct ft_ctx *ctx)
{
	unsigned char buf[2] = {0};

	ft_read(ctx, buf, 2);
	return (uint32_t)buf[0] | ((uint32_t)buf[1]<<8);
}

//////////////////////////////////////////////////////////////////////////////
// Unreduce
//////////////////////////////////////////////////////////////////////////////

static size_t my_ozur_read(ozur_ctx *ozur, OZUR_UINT8 *buf, size_t size)
{
	struct ft_ctx *ctx = (struct ft_ctx*)ozur->userdata;
	return ft_read(ctx, buf, size);
}

static size_t my_ozur_write(ozur_ctx *ozur, const OZUR_UINT8 *buf, size_t size)
{
	struct ft_ctx *ctx = (struct ft_ctx*)ozur->userdata;
	return ft_write(ctx, buf, size);
}

static void unreduce_member_file(struct ft_ctx *ctx,
	off_t csize, off_t ucsize, unsigned int cmpr_factor)
{
	ozur_ctx *ozur = NULL;

	ozur = (ozur_ctx *)calloc(1, sizeof(ozur_ctx));
	if(!ozur) goto done;

	ozur->userdata = (void*)ctx;
	ozur->cmpr_size = csize;
	ozur->uncmpr_size = ucsize;
	ozur->cmpr_factor = cmpr_factor;
	ozur->cb_read = my_ozur_read;
	ozur->cb_write = my_ozur_write;

	ozur_run(ozur);
	if(ozur->error_code != OZUR_ERRCODE_OK) {
		fprintf(stderr, "Decompression failed (code %d)\n", ozur->error_code);
	}

done:
	free(ozur);
}

//////////////////////////////////////////////////////////////////////////////
// Unimplode
//////////////////////////////////////////////////////////////////////////////

static size_t my_ui6a_read(ui6a_ctx *ui6a, UI6A_UINT8 *buf, size_t size)
{
	struct ft_ctx *ctx = (struct ft_ctx*)ui6a->userdata;
	return ft_read(ctx, buf, size);
}

static size_t my_ui6a_write(ui6a_ctx *ui6a, const UI6A_UINT8 *buf, size_t size)
{
	struct ft_ctx *ctx = (struct ft_ctx*)ui6a->userdata;
	return ft_write(ctx, buf, size);
}

static void unimplode_member_file(struct ft_ctx *ctx,
	off_t csize, off_t ucsize, uint16_t bit_flags)
{
	ui6a_ctx *ui6a = NULL;

	ui6a = ui6a_create((void*)ctx);
	if(!ui6a) goto done;

	ui6a->cmpr_size = csize;
	ui6a->uncmpr_size = ucsize;
	ui6a->bit_flags = bit_flags;
	ui6a->cb_read = my_ui6a_read;
	ui6a->cb_write = my_ui6a_write;

	ui6a_unimplode(ui6a);
	if(ui6a->error_code != UI6A_ERRCODE_OK) {
		fprintf(stderr, "Decompression failed (code %d)\n", ui6a->error_code);
	}

done:
	ui6a_destroy(ui6a);
}


struct member_file_data {
	off_t local_file_header_pos;
	off_t local_file_header_len;
	off_t data_offset;
	off_t csize;
	off_t ucsize;
	uint16_t bit_flags;
	uint16_t cmpr_method;
};

static void process_member_file(struct ft_ctx *ctx, struct member_file_data *mfd)
{
	uint32_t sig;

	// Read the local file header
	ft_seek_set(ctx, mfd->local_file_header_pos);

	sig = readui32(ctx);
	if(sig != 0x4034b50U) {
		fprintf(stderr, "Failed to find local file header\n");
		goto done;
	}

	skipbytes(ctx, 2); // version-needed
	mfd->bit_flags = readui16(ctx);
	mfd->cmpr_method = readui16(ctx);

	fprintf(stderr, "Compression method: %u\n", (unsigned int)mfd->cmpr_method);
	if(mfd->cmpr_method<2 || mfd->cmpr_method>6) {
		fprintf(stderr, "Compression method %u not supported\n", (unsigned int)mfd->cmpr_method);
		goto done;
	}

	skipbytes(ctx, 8); // time/date/crc
	mfd->csize = readui32(ctx);
	fprintf(stderr, "Found %u bytes of compressed data\n", (unsigned int)mfd->csize);
	mfd->ucsize = readui32(ctx);

	mfd->local_file_header_len = 30;
	mfd->local_file_header_len += readui16(ctx); // file name len
	mfd->local_file_header_len += readui16(ctx); // extra field len

	mfd->data_offset = mfd->local_file_header_pos + mfd->local_file_header_len;

	ft_seek_set(ctx, mfd->data_offset);

	if(mfd->cmpr_method>=2 && mfd->cmpr_method<=5) {
		unreduce_member_file(ctx, mfd->csize,
			mfd->ucsize, (unsigned int)(mfd->cmpr_method - 1));
	}
	else if(mfd->cmpr_method==6) {
		unimplode_member_file(ctx, mfd->csize,
			mfd->ucsize, mfd->bit_flags);
	}

done:
	;
}

static void read_input_data(struct ft_ctx *ctx)
{
	ctx->inbuf_pos = 0;

	while(1) {
		size_t ret;

		if(feof(stdin)) break;
		if(ferror(stdin)) break;
		if(ctx->inbuf_pos >= FT_MAX_INPUT_SIZE) break;
		ret = fread(&ctx->inbuf[ctx->inbuf_pos], 1,
			FT_MAX_INPUT_SIZE - ctx->inbuf_pos, stdin);
		if(ret==0) break;
		ctx->inbuf_pos += ret;
	}
	ctx->inbuf_size = ctx->inbuf_pos;
	if(ctx->inbuf_size > FT_MAX_INPUT_SIZE) {
		ctx->inbuf_size = FT_MAX_INPUT_SIZE;
	}

	fprintf(stderr, "Read %u bytes of ZIP input\n",
		(unsigned int)ctx->inbuf_size);
}

int main(int argc, char **argv)
{
	struct ft_ctx *ctx = NULL;
	struct member_file_data mfd = {0};

	ctx = malloc(sizeof(struct ft_ctx));
	if(!ctx) goto done;

	ctx->inbuf_pos = 0;
	ctx->inbuf_size = 0;
	ctx->nbytes_emitted = 0;

	read_input_data(ctx);

	mfd.local_file_header_pos = 0;
	process_member_file(ctx, &mfd);

	fprintf(stderr, "Decompressed to %u bytes\n",
		(unsigned int)ctx->nbytes_emitted);

done:
	free(ctx);
	return 0;
}
