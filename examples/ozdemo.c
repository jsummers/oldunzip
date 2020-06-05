// Demonstration program for the Ozunreduce and Unimplode6a libraries.
// Written by Jason Summers, 2019-2020.
/*
======================== TERMS OF USE for ozdemo.c =========================
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
#include "ozunshrink.h"

#include "ozcrc32.h"

#define DEMO_FSEEK fseek
#define DEMO_FTELL ftell

struct demo_userdata {
	FILE *inf;
	FILE *outf;
	uint32_t crc_c;
};

// It is intentional that the next three sections have some repeated code.
// The idea is to demonstrate using each library independently.

//////////////////////////////////////////////////////////////////////////////
// Unshrink
//////////////////////////////////////////////////////////////////////////////

static size_t my_ozus_read(ozus_ctx *ozus, OZUS_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ozus->userdata;
	return (size_t)fread(buf, 1, size, u->inf);
}

static size_t my_ozus_write(ozus_ctx *ozus, const OZUS_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ozus->userdata;
	u->crc_c = ozcrc32(buf, size, u->crc_c);
	return (size_t)fwrite(buf, 1, size, u->outf);
}

static void unshrink_member_file(FILE *inf, off_t data_offset,
	off_t csize, off_t ucsize, uint32_t crc_r, char *outfn)
{
	ozus_ctx *ozus = NULL;
	FILE *outf = NULL;
	struct demo_userdata u;

	outf = fopen(outfn, "wb");
	if(!outf) {
		printf("Open for write failed\n");
		goto done;
	}
	printf("Extracting to %s\n", outfn);

	u.inf = inf;
	u.outf = outf;
	u.crc_c = 0;

	ozus = (ozus_ctx *)calloc(1, sizeof(ozus_ctx));
	if(!ozus) goto done;

	ozus->userdata = (void*)&u;
	ozus->cmpr_size = csize;
	ozus->uncmpr_size = ucsize;
	ozus->cb_read = my_ozus_read;
	ozus->cb_write = my_ozus_write;
	DEMO_FSEEK(inf, (long)data_offset, SEEK_SET);

	ozus_run(ozus);
	if(ozus->error_code != OZUS_ERRCODE_OK) {
		printf("Decompression failed (code %d)\n", ozus->error_code);
		goto done;
	}

	printf("CRC-32, calculated: 0x%08x\n", (unsigned int)u.crc_c);
	if(u.crc_c != crc_r) {
		printf("CRC check failed\n");
	}

done:
	if(outf) fclose(outf);
	free(ozus);
}

//////////////////////////////////////////////////////////////////////////////
// Unreduce
//////////////////////////////////////////////////////////////////////////////

static size_t my_ozur_read(ozur_ctx *ozur, OZUR_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ozur->userdata;
	return (size_t)fread(buf, 1, size, u->inf);
}

static size_t my_ozur_write(ozur_ctx *ozur, const OZUR_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ozur->userdata;
	u->crc_c = ozcrc32(buf, size, u->crc_c);
	return (size_t)fwrite(buf, 1, size, u->outf);
}

static void unreduce_member_file(FILE *inf, off_t data_offset,
	off_t csize, off_t ucsize, unsigned int cmpr_factor, uint32_t crc_r,
	char *outfn)
{
	ozur_ctx *ozur = NULL;
	FILE *outf = NULL;
	struct demo_userdata u;

	outf = fopen(outfn, "wb");
	if(!outf) {
		printf("Open for write failed\n");
		goto done;
	}
	printf("Extracting to %s\n", outfn);

	u.inf = inf;
	u.outf = outf;
	u.crc_c = 0;

	ozur = (ozur_ctx *)calloc(1, sizeof(ozur_ctx));
	if(!ozur) goto done;

	ozur->userdata = (void*)&u;
	ozur->cmpr_size = csize;
	ozur->uncmpr_size = ucsize;
	ozur->cmpr_factor = cmpr_factor;
	ozur->cb_read = my_ozur_read;
	ozur->cb_write = my_ozur_write;
	DEMO_FSEEK(inf, (long)data_offset, SEEK_SET);

	ozur_run(ozur);
	if(ozur->error_code != OZUR_ERRCODE_OK) {
		printf("Decompression failed (code %d)\n", ozur->error_code);
		goto done;
	}

	printf("CRC-32, calculated: 0x%08x\n", (unsigned int)u.crc_c);
	if(u.crc_c != crc_r) {
		printf("CRC check failed\n");
	}

done:
	if(outf) fclose(outf);
	free(ozur);
}

//////////////////////////////////////////////////////////////////////////////
// Unimplode
//////////////////////////////////////////////////////////////////////////////

static size_t my_ui6a_read(ui6a_ctx *ui6a, UI6A_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ui6a->userdata;

	// Note: Returning anything other than the requested 'size' is considered
	// a failure.
	return (size_t)fread(buf, 1, size, u->inf);
}

static size_t my_ui6a_write(ui6a_ctx *ui6a, const UI6A_UINT8 *buf, size_t size)
{
	struct demo_userdata *u = (struct demo_userdata*)ui6a->userdata;

	u->crc_c = ozcrc32(buf, size, u->crc_c);
	// Note: Returning anything other than the requested 'size' is considered
	// a failure.
	return (size_t)fwrite(buf, 1, size, u->outf);
}

static void unimplode_member_file(FILE *inf, off_t data_offset,
	off_t csize, off_t ucsize, uint16_t bit_flags, uint32_t crc_r,
	char *outfn)
{
	ui6a_ctx *ui6a = NULL;
	FILE *outf = NULL;
	struct demo_userdata u;

	outf = fopen(outfn, "wb");
	if(!outf) {
		printf("Open for write failed\n");
		goto done;
	}
	printf("Extracting to %s\n", outfn);

	u.inf = inf;
	u.outf = outf;
	u.crc_c = 0;

	ui6a = ui6a_create((void*)&u);
	if(!ui6a) goto done;

	ui6a->cmpr_size = csize;
	ui6a->uncmpr_size = ucsize;
	ui6a->bit_flags = bit_flags;
	ui6a->cb_read = my_ui6a_read;
	ui6a->cb_write = my_ui6a_write;

	DEMO_FSEEK(inf, (long)data_offset, SEEK_SET);
	ui6a_unimplode(ui6a);
	if(ui6a->error_code != UI6A_ERRCODE_OK) {
		printf("Decompression failed (code %d)\n", ui6a->error_code);
	}

	printf("CRC-32, calculated: 0x%08x\n", (unsigned int)u.crc_c);
	if(u.crc_c != crc_r) {
		printf("CRC check failed\n");
	}

done:
	ui6a_destroy(ui6a);
	if(outf) fclose(outf);
}

//////////////////////////////////////////////////////////////////////////////
// Compression method 0 (no compression)
// For completion, so that this demo supports all PKZIP 1.x and earlier
// archives.
//////////////////////////////////////////////////////////////////////////////

static void copy_member_file(FILE *inf, off_t data_offset,
	off_t csize, off_t ucsize, uint32_t crc_r,
	char *outfn)
{
	FILE *outf = NULL;
	off_t nbytes_remaining = csize;
	uint32_t crc_c = 0;

	outf = fopen(outfn, "wb");
	if(!outf) {
		printf("Open for write failed\n");
		goto done;
	}
	printf("Extracting to %s\n", outfn);

	DEMO_FSEEK(inf, (long)data_offset, SEEK_SET);

	while(nbytes_remaining >= 1) {
		int ci;
		unsigned char ch;

		ci = fgetc(inf);
		if(ci==EOF) break;
		fputc(ci, outf);
		ch = (unsigned char)ci;
		crc_c = ozcrc32(&ch, 1, crc_c);
		nbytes_remaining--;
	}

	printf("CRC-32, calculated: 0x%08x\n", (unsigned int)crc_c);
	if(crc_c != crc_r) {
		printf("CRC check failed\n");
	}

done:
	if(outf) fclose(outf);
}

//////////////////////////////////////////////////////////////////////////////
// Below this point is just code for ZIP parsing, nothing specific to
// the libraries.
//
// (Due to the complexity of ZIP format, the minimal useful sample
// program is fairly large.)
//////////////////////////////////////////////////////////////////////////////

// Feel free to change these settings:

// It's just a demo. Let's not create hundreds of files.
#define DEMO_MAX_FILES_TO_EXTRACT 25
// Sanity check. Stop if we see a file bigger than this.
#define DEMO_MAX_FILE_SIZE 50000000
// Sanity check. Stop if the archive has more files than this.
#define DEMO_MAX_FILES 1000
// How far back to look for the end-of-central-dir signature.
#define DEMO_MAX_SIG_SEARCH 70000

#define DEMO_SNPRINTF snprintf

struct zip_file_data {
	FILE *inf;
	off_t central_dir_pos;
	uint32_t num_member_files;
	int num_files_extracted;
};

struct member_file_data {
	off_t central_dir_entry_pos;
	off_t central_dir_entry_len;
	off_t local_file_header_pos;
	off_t local_file_header_len;
	int idx;
	off_t data_offset;
	off_t csize;
	off_t ucsize;
	uint16_t bit_flags;
	uint16_t cmpr_method;
	uint32_t crc_r;
};

static void skipbytes(struct zip_file_data *zfd, off_t n)
{
	DEMO_FSEEK(zfd->inf, n, SEEK_CUR);
}

static uint32_t readui32(struct zip_file_data *zfd)
{
	unsigned char buf[4] = {0};

	fread(buf, 1, 4, zfd->inf);
	return (uint32_t)buf[0] | ((uint32_t)buf[1]<<8) |
		((uint32_t)buf[2]<<16) | ((uint32_t)buf[3]<<24);
}

static uint32_t readui16(struct zip_file_data *zfd)
{
	unsigned char buf[2] = {0};

	fread(buf, 1, 2, zfd->inf);
	return (uint32_t)buf[0] | ((uint32_t)buf[1]<<8);
}

static int process_member_file(struct zip_file_data *zfd, struct member_file_data *mfd)
{
	uint32_t sig;
	int retval = 0;
	char outfn[512];

	printf("Member file #%d\n", mfd->idx);
	printf("Central dir entry offset: %u\n", (unsigned int)mfd->central_dir_entry_pos);

	DEMO_FSEEK(zfd->inf, mfd->central_dir_entry_pos, SEEK_SET);

	sig = readui32(zfd);
	if(sig != 0x2014b50U) {
		printf("Parse error\n");
		goto done;
	}

	skipbytes(zfd, 4); // version-made, version-needed

	mfd->bit_flags = readui16(zfd);
	printf("Bit flags: 0x%04x\n", (unsigned int)mfd->bit_flags);
	mfd->cmpr_method = readui16(zfd);
	printf("Compression method: %u\n", (unsigned int)mfd->cmpr_method);

	skipbytes(zfd, 4); // time, date

	mfd->crc_r = readui32(zfd);
	printf("CRC-32, reported: 0x%08x\n", (unsigned int)mfd->crc_r);
	mfd->csize = readui32(zfd);
	printf("Compressed size: %u\n", (unsigned int)mfd->csize);
	mfd->ucsize = readui32(zfd);
	printf("Uncompressed size: %u\n", (unsigned int)mfd->ucsize);

	mfd->central_dir_entry_len = 46;
	mfd->central_dir_entry_len += readui16(zfd); // file name len
	mfd->central_dir_entry_len += readui16(zfd); // extra field len
	mfd->central_dir_entry_len += readui16(zfd); // file comment len

	skipbytes(zfd, 8); // disk number, internal attrs, external attrs
	mfd->local_file_header_pos = readui32(zfd);
	printf("Local file header offset: %u\n", (unsigned int)mfd->local_file_header_pos);

	if(mfd->ucsize > DEMO_MAX_FILE_SIZE) {
		printf("Max file size exceeded\n");
		goto done;
	}

	// We think we've parsed the central dir entry successfully, so return "success".
	retval = 1;

	if(mfd->cmpr_method<0 || mfd->cmpr_method>6) {
		printf("Compression method %u not supported\n", (unsigned int)mfd->cmpr_method);
		goto done;
	}

	// Now read the local file header
	DEMO_FSEEK(zfd->inf, mfd->local_file_header_pos, SEEK_SET);
	sig = readui32(zfd);
	if(sig != 0x4034b50U) {
		printf("Failed to find local file header\n");
		goto done;
	}
	skipbytes(zfd, 22); // version-needed, through uncompressed-size

	mfd->local_file_header_len = 30;
	mfd->local_file_header_len += readui16(zfd); // file name len
	mfd->local_file_header_len += readui16(zfd); // extra field len

	mfd->data_offset = mfd->local_file_header_pos + mfd->local_file_header_len;
	printf("Compressed data offset: %u\n", (unsigned int)mfd->data_offset);

	DEMO_SNPRINTF(outfn, sizeof(outfn), "demo.%03d.out", mfd->idx);

	if(mfd->cmpr_method==0) {
		copy_member_file(zfd->inf, mfd->data_offset, mfd->csize,
			mfd->ucsize, mfd->crc_r, outfn);
	}
	else if(mfd->cmpr_method==1) {
		unshrink_member_file(zfd->inf, mfd->data_offset, mfd->csize,
			mfd->ucsize, mfd->crc_r, outfn);
	}
	else if(mfd->cmpr_method>=2 && mfd->cmpr_method<=5) {
		unreduce_member_file(zfd->inf, mfd->data_offset, mfd->csize,
			mfd->ucsize, (unsigned int)(mfd->cmpr_method - 1),
			mfd->crc_r, outfn);
	}
	else if(mfd->cmpr_method==6) {
		unimplode_member_file(zfd->inf, mfd->data_offset, mfd->csize,
			mfd->ucsize, mfd->bit_flags, mfd->crc_r, outfn);
	}
	zfd->num_files_extracted++;

done:
	return retval;
}

static int find_central_dir(struct zip_file_data *zfd)
{
	off_t search_start;
	off_t search_len;
	off_t file_size;
	long k;
	unsigned char *tmpbuf = NULL;
	int found_eocd = 0;
	int found_central_dir = 0;
	off_t eocd_offset;

	DEMO_FSEEK(zfd->inf, 0, SEEK_END);
	file_size = DEMO_FTELL(zfd->inf);
	if(file_size<22) goto done;

	search_start = 0;
	search_len = file_size - 18;
	if(search_len > DEMO_MAX_SIG_SEARCH) {
		search_start += (search_len - DEMO_MAX_SIG_SEARCH);
		search_len = DEMO_MAX_SIG_SEARCH;
	}

	printf("Searching for EOCD record (start=%u, len=%u)\n", (unsigned int)search_start,
		(unsigned int)search_len);
	DEMO_FSEEK(zfd->inf, search_start, SEEK_SET);
	tmpbuf = (unsigned char*)malloc(search_len);
	fread(tmpbuf, 1, search_len, zfd->inf);

	for(k=search_len-4; k>=0; k--) {
		if(tmpbuf[k]=='P' && tmpbuf[k+1]=='K' && tmpbuf[k+2]==5 && tmpbuf[k+3]==6) {
			found_eocd = 1;
			eocd_offset = search_start + k;
			break;
		}
	}

	if(!found_eocd) {
		goto done;
	}

	printf("EOCD record offset: %u\n", (unsigned int)eocd_offset);
	DEMO_FSEEK(zfd->inf, eocd_offset+8, SEEK_SET);
	zfd->num_member_files = readui16(zfd);
	printf("Number of member files: %u\n", (unsigned int)zfd->num_member_files);

	skipbytes(zfd, 6);
	zfd->central_dir_pos = readui32(zfd);
	printf("Central dir offset: %u\n", (unsigned int)zfd->central_dir_pos);
	found_central_dir = 1;

done:
	free(tmpbuf);
	return found_central_dir;
}

int main(int argc, char **argv)
{
	struct zip_file_data zfd = {0};
	int idx;
	const char *input_filename;
	off_t cur_offset;

	if(argc!=2) {
		printf("Usage: ui6ademo <input-file.zip>\n");
		goto done;
	}
	input_filename = argv[1];

	zfd.inf = fopen(input_filename, "rb");
	if(!zfd.inf) {
		printf("Open for read failed\n");
		goto done;
	}

	if(!find_central_dir(&zfd)) {
		printf("Not a ZIP file\n");
		goto done;
	}

	if(zfd.num_member_files>DEMO_MAX_FILES) {
		printf("Max number of files exceeded\n");
		goto done;
	}

	cur_offset = zfd.central_dir_pos;

	for(idx=0; idx<(int)zfd.num_member_files; idx++) {
		struct member_file_data mfd = {0};

		printf("\n");

		mfd.central_dir_entry_pos = cur_offset;
		mfd.idx = idx;

		if(!process_member_file(&zfd, &mfd)) {
			goto done;
		}

		if(zfd.num_files_extracted >= DEMO_MAX_FILES_TO_EXTRACT) {
			printf("Reached max number of files that this demo extracts (%d). Stopping.\n",
				DEMO_MAX_FILES_TO_EXTRACT);
			break;
		}

		cur_offset += mfd.central_dir_entry_len;
	}

done:
	if(zfd.inf) fclose(zfd.inf);
	return 0;
}
