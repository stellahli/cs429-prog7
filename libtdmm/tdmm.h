#ifndef TDMM_H
#define TDMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define META_SIZE 24

typedef struct Block {
	uint size;				// 4 bytes, size w/o metadata
	int allocated; 			// 4 bytes, 1 is allocated, 0 is free
	struct Block *next;		// 8 bytes
	struct Block *prev; 	// 8 bytes
} block_t;

typedef enum {
  FIRST_FIT,
  BEST_FIT,
  WORST_FIT,
  BUDDY,
  MIXED
} alloc_strat_e;

/* internal allocator state (defined in tdmm.c) */
extern size_t bytes_requested;
extern uint total_blocks;

/* heap_head and alloc_strat are kept private to tdmm.c */

// helper methods
int check_allocate(block_t *block, uint size);
void allocate(block_t *block, uint size);
void expand(block_t *end, size_t size);
void check_free(block_t *block);

// buddy helper methods
uint check_allocate_buddy(block_t *block , size_t size);
void break_block(block_t *block, int break_number);
void free_check_buddy(block_t *block);
size_t get_power_of_2(size_t number);

// testing helper methods
double get_mem_util();
uint get_overhead();

/**
 * Initializes the memory allocator with the given strategy.
 *
 * @param strat The strategy to use for memory allocation.
 */
void t_init(alloc_strat_e strat);

/**
 * Allocates a block of memory of the given size.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block fails.
 */
void *t_malloc(size_t size);

/**
 * Frees the given memory block.
 *
 * @param ptr The pointer to the memory block to free. This must be a pointer returned by t_malloc.
 */
void t_free(void *ptr);

#endif // TDMM_H
