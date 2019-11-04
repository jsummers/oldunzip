// A program to test that the example code in unimplode6a.h works.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define OZUS_TESTING_EXAMPLE_CODE
#include "ozunshrink.h"
#define OZUR_TESTING_EXAMPLE_CODE
#include "ozunreduce.h"
#define UI6A_TESTING_EXAMPLE_CODE
#include "unimplode6a.h"

static int test_ozunshrink(void)
{
	FILE *infile, *outfile;
	const char *infn = "example-s.zip";
	long cmpr_offset = 35;
	OZUS_OFF_T cmpr_size = 459;
	const char *outfn = "example-s.out";
	OZUS_OFF_T uncmpr_size = 2048;

	printf("[ozunshrink]\n");
	infile = fopen(infn, "rb");
	if(!infile) {
		return 0;
	}
	outfile = fopen(outfn, "wb");
	if(!outfile) {
		fclose(infile);
		return 0;
	}
	fseek(infile, cmpr_offset, SEEK_SET);
	ozus_example_code(infile, outfile, cmpr_size, uncmpr_size);
	fclose(outfile);
	fclose(infile);
	printf("Wrote %s.\n", outfn);
	return 1;
}

static int test_ozunreduce(void)
{
	FILE *infile, *outfile;
	const char *infn = "example-r.zip";
	long cmpr_offset = 35;
	OZUR_OFF_T cmpr_size = 276;
	unsigned int cmpr_factor = 4;
	const char *outfn = "example-r.out";
	OZUR_OFF_T uncmpr_size = 2048;

	printf("[ozunreduce]\n");
	infile = fopen(infn, "rb");
	if(!infile) {
		return 0;
	}
	outfile = fopen(outfn, "wb");
	if(!outfile) {
		fclose(infile);
		return 0;
	}
	fseek(infile, cmpr_offset, SEEK_SET);
	ozur_example_code(infile, outfile, cmpr_size, uncmpr_size, cmpr_factor);
	fclose(outfile);
	fclose(infile);
	printf("Wrote %s.\n", outfn);
	return 1;
}

static int test_unimplode6a(void)
{
	FILE *infile, *outfile;
	const char *infn = "example-i.zip";
	long cmpr_offset = 35;
	UI6A_OFF_T cmpr_size = 89;
	UI6A_UINT16 bit_flags = UI6A_FLAG_4KDICT | UI6A_FLAG_2TREES;
	const char *outfn = "example-i.out";
	UI6A_OFF_T uncmpr_size = 2048;

	printf("[unimplode6a]\n");
	infile = fopen(infn, "rb");
	if(!infile) {
		return 0;
	}
	outfile = fopen(outfn, "wb");
	if(!outfile) {
		fclose(infile);
		return 0;
	}
	fseek(infile, cmpr_offset, SEEK_SET);
	ui6a_example_code(infile, outfile, cmpr_size, uncmpr_size, bit_flags);
	fclose(outfile);
	fclose(infile);
	printf("Wrote %s.\n", outfn);
	return 1;
}

int main(int argc, char **argv)
{
	int ret1, ret2, ret3;

	ret1 = test_ozunshrink();
	ret2 = test_ozunreduce();
	ret3 = test_unimplode6a();
	return (ret1 && ret2 && ret3) ? 0 : 1;
}
