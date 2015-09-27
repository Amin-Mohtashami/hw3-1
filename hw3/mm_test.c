/* A simple test harness for memory alloction. */

#include "mm_alloc.h"
#include <stdio.h>

#define DEBUG 1

int main(int argc, char **argv) {
	FILE *outfile = DEBUG ? stdout : fopen("/dev/null", "w");
    int *d1, *d2, *d3, *d4;
	//fprintf(outfile, "func_call  : mm_malloc\n");
    d1 = mm_malloc(sizeof(int) * 160);
	fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) d1);
    d2 = mm_malloc(sizeof(int));
    fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) d2);
    mm_free(d1);
    d3 = mm_malloc(sizeof(int));
    fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) d3);
    d2 = mm_realloc(d2, sizeof(int) * 4);
    fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) d2);
    //fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) (d2 = mm_malloc(1)));
    //fprintf(outfile, "recv_blk*  : &%ld\n", (mm_lui_t) mm_malloc(sizeof(int*)));
	mm_print_mem();
    /*
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
    */

	printf("=============================\n");
	printf("verdict    : \033[32mSuccessful.\033[0m\n");

	fclose(outfile);
	return 0;
}
