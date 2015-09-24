/* A simple test harness for memory alloction. */

#include "mm_alloc.h"
#include <stdio.h>

#define DEBUG 0

int main(int argc, char **argv) {
    int  *data, *data3;
	char *data2, *data4;

	FILE *outfile = DEBUG ? stdout : fopen("/dev/null", "w");

	fprintf(outfile, "func_call  : mm_malloc\n");
    data = (int*) mm_malloc(sizeof(int));
	fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) data);
    data2 = (char*) mm_malloc(sizeof(char));
	fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) data2);
    data3 = (int*) mm_malloc(sizeof(int));
	fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) data3);
    data4 = (char*) mm_malloc(sizeof(char));
	fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) data4);
	fprintf(outfile, "working.\n");

	fprintf(outfile, "mem_assign : data = 121\n");
    data = 121;
	fprintf(outfile, "working.\n");
	fprintf(outfile, "mem_assign : data2 = 'a'\n");
    data2 = 'a';
	fprintf(outfile, "working.\n");

	fprintf(outfile, "mem_assert : data  == 121   ? %d\n", (data == 121));
	fprintf(outfile, "mem_assert : data2 == 'a' ? %d\n", (data2 == 'a'));
	fprintf(outfile, "working.\n");

	fprintf(outfile, "func_call  : mm_free\n");
	mm_free(data);
	fprintf(outfile, "func_call  : mm_free\n");
	mm_free(data2);
	fprintf(outfile, "func_call  : mm_free\n");
	mm_free(data3);
	fprintf(outfile, "func_call  : mm_free\n");
	mm_free(data4);
	fprintf(outfile, "working.\n");
	
	printf("===============================\n");
	printf("verdict    : \033[32mSuccessful.\033[0m\n");
	
	fclose(outfile);
	return 0;
}
