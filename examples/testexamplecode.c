// A program to test that the example code in unimplode6a.h works.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define UI6A_TESTING_EXAMPLE_CODE
#include "unimplode6a.h"

int main(int argc, char **argv)
{
	FILE *infile, *outfile;
	const char *infn = "example.zip";
	long cmpr_offset = 37;
	UI6A_OFF_T cmpr_size = 28;
	UI6A_UINT16 bit_flags = UI6A_FLAG_4KDICT | UI6A_FLAG_2TREES;
	const char *outfn = "example.out";
	UI6A_OFF_T uncmpr_size = 29;

	infile = fopen(infn, "rb");
	if(!infile) {
		return 1;
	}
	outfile = fopen(outfn, "wb");
	if(!outfile) {
		fclose(infile);
		return 1;
	}
	fseek(infile, cmpr_offset, SEEK_SET);
	ui6a_example_code(infile, outfile, cmpr_size, uncmpr_size, bit_flags);
	fclose(outfile);
	fclose(infile);
	printf("Wrote %s.\n", outfn);
	return 0;
}
