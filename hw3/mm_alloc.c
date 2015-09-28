/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/* Your final implementation should comment out this macro. */
// #define MM_USE_STUBS

s_blk_ptr MEM_ALLOC_ROOT = NULL;
mm_lui_t heap_start;

s_blk_ptr extend_heap(s_blk_ptr last, size_t s) {
	s_blk_ptr _temp = sbrk(sizeof(struct s_blk) + s);
	_temp->size = s;
	_temp->prev = last;
	_temp->next = NULL;
	_temp->free = 0;
	_temp->ptr  = (void*) ((mm_lui_t) _temp + (mm_lui_t) sizeof(struct s_blk));

	return _temp;
}

void split_block(s_blk_ptr b, size_t s) {
	s_blk_ptr _temp = (s_blk_ptr) ((mm_lui_t) b + sizeof(struct s_blk) + s);
	
	size_t _req = b->size - s - sizeof(struct s_blk);
	_temp->size = _req;

	_temp->prev = b;
	_temp->next = b->next;
	b->next = _temp;
	b->size = s;

	_temp->free = 1;
	_temp->ptr  = (void*) ((mm_lui_t) _temp + (mm_lui_t) sizeof(struct s_blk));
}

void* mm_malloc(size_t size) {
#ifdef MM_USE_STUBS
    return calloc(1, size);
#else
	// Check if any memory has been allocated before
	if (MEM_ALLOC_ROOT == NULL) {
		// define heap_start
		heap_start = (mm_lui_t) sbrk(0);
		// Assign memory for first mem block
		MEM_ALLOC_ROOT = extend_heap(NULL, size);
	}
	else {
		// check if first block is free
		MEM_ALLOC_ROOT = (s_blk_ptr) heap_start;
		s_blk_ptr *_curr = &MEM_ALLOC_ROOT;
		s_blk_ptr *_prev = (s_blk_ptr*) (*_curr)->prev;

		while ((*_curr) != NULL) {
			if ((*_curr)->free && size <= (*_curr)->size) {
				// blk is empty and is big enough,
				// so return mem pointer and flip the free value
				(*_curr)->free = 0;

				//if this free block has enough free space, split it
				// enough free space: so there's enough space to fit this
				// request, and a whole block with at least 1 byte of actual
				// dyn_alloc space.
				size_t _req = sizeof(struct s_blk) + size + 1;
				if ((*_curr)->size >= _req) {
					split_block((*_curr), size);
				}

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
		(*_prev)->next = extend_heap(*_prev, size);
		
		return (*_prev)->next->ptr;
	}

	return MEM_ALLOC_ROOT->ptr;
#endif
}

void* mm_realloc(void* ptr, size_t size) {
#ifdef MM_USE_STUBS
    return realloc(ptr, size);
#else
	// TODO: if size == ptr->size then return ptr;
	s_blk_ptr _old_ptr = (s_blk_ptr) ((mm_lui_t) ptr - sizeof(struct s_blk));
	_old_ptr->free = 1;	

	s_blk_ptr _new = mm_malloc(size); 
	memmove(_new, ptr, size);

	return _new;
#endif
}

// fuse two consecutive memory blk's
s_blk_ptr fusion(s_blk_ptr ptr) {
 	// check if there are any free blks on either side
	if (ptr->prev != NULL && ptr->prev->free) {
		ptr->prev->next = ptr->next;
		ptr->prev->size = ptr->prev->size + sizeof(struct s_blk) + ptr->size;
		return ptr->prev;
	}
	if (ptr->next != NULL && ptr->next->free) {
		ptr->next = ptr->next->next;
		ptr->size = ptr->size + sizeof(struct s_blk) + ptr->next->size;
	}

	return ptr;
}

void mm_free(void* ptr) {
#ifdef MM_USE_STUBS
    free(ptr);
#else
	// check if first block is free
	MEM_ALLOC_ROOT = (s_blk_ptr) heap_start;
	s_blk_ptr *_curr = &MEM_ALLOC_ROOT;

	while ((*_curr) != NULL) {
		if ((*_curr)->ptr == ptr) {
			(*_curr)->free = 1;
			fusion(*_curr);
			break;
		}

		// check if the next is null before assigning...
		if ((*_curr)->next == NULL) break;
		*_curr = (s_blk_ptr) (*_curr)->next;
	}
#endif
}

void mm_print_mem() {
	//return;
	fprintf(stdout, "\nstart_addr\tsize\tfree\n");
	fprintf(stdout, "=============================\n");

	// check if first block is free
	MEM_ALLOC_ROOT = (s_blk_ptr) heap_start;
	s_blk_ptr *_curr = &MEM_ALLOC_ROOT;
	int i = 0;

	while ((*_curr) != NULL  && i <= 10) {
		fprintf(stdout, "%ld\t%ld\t%d\n", (mm_lui_t) (*_curr), (*_curr)->size,
			(*_curr)->free);
		// check if the next is null before assigning...
		if ((*_curr)->next == NULL) break;
		*_curr = (s_blk_ptr) (*_curr)->next;
		i++;
	}
}
