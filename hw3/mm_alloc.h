/*
 * mm_alloc.h
 *
 * Exports a clone of the interface documented in "man 3 malloc".
 */

#pragma once

#ifndef _malloc_H_
#define _malloc_H_

 /* Define the block size since the sizeof will be wrong */
#define BLOCK_SIZE 40

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>

void* mm_malloc(size_t size);
void* mm_realloc(void* ptr, size_t size);
void mm_free(void* ptr);


typedef struct s_blk* s_blk_ptr;
typedef long unsigned int mm_lui_t;

/* block struct */
struct s_blk {
    size_t size;
    s_blk_ptr next;
    s_blk_ptr prev;
    int free;
    void *ptr;
    /* A pointer to the allocated block */
    char data [0];
 };

/* Split block according to size, b must exist */
void split_block (s_blk_ptr b, size_t s);

/* Try fusing block with neighbors */
s_blk_ptr fusion(s_blk_ptr b);

/* Get the block from addr */
s_blk_ptr get_blk (void *p);

/* Add a new block at the of heap,
 * return NULL if things go wrong
 */
s_blk_ptr extend_heap (s_blk_ptr last , size_t s);

/*
 * Show current mem structure
 */
void mm_print_mem();

#ifdef __cplusplus
}
#endif

#endif
