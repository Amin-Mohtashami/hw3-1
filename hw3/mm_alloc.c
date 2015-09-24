/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/* Your final implementation should comment out this macro. */
// #define MM_USE_STUBS

s_blk_ptr mem_alloc_root = NULL;
mm_lui_t heap_start;

s_blk_ptr _new_mem_blk(void* prev_blk, size_t size) {
	s_blk_ptr _temp = sbrk(sizeof(s_blk_ptr) + size + 1);
	_temp->size = size;
	_temp->prev = (s_blk_ptr) (prev_blk);
	_temp->next = NULL;
	_temp->free = 0;
	_temp->ptr  = (void*) ((mm_lui_t) _temp + (mm_lui_t) sizeof(s_blk_ptr));	
	fprintf(stdout, "new_blk_ptr (start) - &%ld\n",(mm_lui_t) _temp);
	fprintf(stdout, "new_blk_ptr (ptr)   - &%ld\n",(mm_lui_t) _temp->ptr);
	fprintf(stdout, "new_blk_ptr (end)   - &%ld\n",(mm_lui_t) _temp->ptr + (mm_lui_t) size);
	return _temp;
}

void* mm_malloc(size_t size) {
#ifdef MM_USE_STUBS
    return calloc(1, size);
#else
	// Check if any memory has been allocated before
	if (mem_alloc_root == NULL) {
		// define heap_start
		heap_start = (mm_lui_t) sbrk(0);
		fprintf(stdout, "size(blk_ptr)       - %ld\n", sizeof(s_blk_ptr));
		fprintf(stdout, "heap_start          - &%ld\n", heap_start);
		fprintf(stdout, "===============================\n");
   
        // Assign memory for first mem block
		mem_alloc_root = _new_mem_blk(NULL, size);
	}
	else {
		// check if first block is free
		s_blk_ptr *_curr = &mem_alloc_root;
		s_blk_ptr *_prev = (s_blk_ptr*) (*_curr)->prev;

		while ((*_curr) != NULL) {
				fprintf(stdout, "...\n");
				//fprintf(stdout, "curr_blk       : &%ld\n", (*_curr));
				//fprintf(stdout, "curr_blk->next : &%ld\n", (*_curr)->next);
				//fprintf(stdout, "curr_blk->prev : &%ld\n", (*_curr)->prev);
				if ((*_curr)->free && size <= (*_curr)->size) {
					// blk is empty and is big enough,
					// so return mem pointer and flip the free value
					(*_curr)->free = 0;
					return (*_curr)->ptr;
				}
				else {
					// check if the next is null before assigning...
					if ((*_curr)->next == NULL) break;
					_prev = _curr;
					*_curr = (s_blk_ptr) (*_curr)->next;
				}
		}

		// if we get here, then all the mem blk's are full or too small
		// make a new blk;
		// Or curr is the first blk and we need to add a next to it
		if (_prev == NULL) _prev = _curr;
		(*_prev)->next = _new_mem_blk((*_prev), size);
		
		return (*_prev)->next->ptr;
	}

	//if (sbrk(size) == (void *) -1)
	//	perror("sbrk");

	return mem_alloc_root->ptr;		
#endif
}

void* mm_realloc(void* ptr, size_t size) {
#ifdef MM_USE_STUBS
    return realloc(ptr, size);
#else
#endif
}

void mm_free(void* ptr) {
#ifdef MM_USE_STUBS
    free(ptr);
#else
	
#endif
}
