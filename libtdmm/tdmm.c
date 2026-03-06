// ASSUMPTION: BLOCK POINTER IS POINTING TO FULL START ADDRESS
// WHERE THE METADATA STARTS

#include "tdmm.h"
#include <limits.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define META_SIZE 32

/* allocator globals */
size_t bytes_requested = 0;
uint total_blocks = 0;
static block_t *heap_head = NULL;
static alloc_strat_e alloc_strat;
int mixed = 0;
int ptr_counter = 0;
void* mmap_ptr[1000];
size_t mmap_regions[1000];

int check_allocate(block_t *block, uint size) {
	if(block->allocated == 1) return 0;
	if(block->size < size) return 0;
	return 1;
}

void allocate(block_t *block, uint size) {
	// perfect size, block becomes allocated
	if(block->size < size + META_SIZE + 4) {
		block->allocated = 1;
		return;
	}

	// makes new free block after allocation
	block_t *new_block = (block_t *) ((char *) block + META_SIZE + size);
	new_block->size = block->size - META_SIZE - size;
	new_block->allocated = 0;
	new_block->prev = block;
	new_block->next = block->next;
	total_blocks += 1; 

	// updates neighbor
	if(new_block->next) {
		new_block->next->prev = new_block;
	}

	//updates original block
	block->size = size;
	block->allocated = 1;
	block->next = new_block;
}

// calls mmap and adds to end
void expand(block_t *end, size_t size) {
	if(end == NULL) return;
	// TO DO : if end is free, check if mmap returns address that connects
	// if end if allocated, just add on
	size_t cur_size = 4096;
	while(cur_size < size + META_SIZE) {
		cur_size += 4096;
	}

	void *ptr = mmap(NULL, cur_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED) {
		fprintf(stderr, "mmap failed");
		exit(1);
	}
	bytes_requested += cur_size;

	// end is free and connects, can't occur for buddy
	if(end->allocated == 0 && ((char *) end + META_SIZE + end->size) == ptr) {
		end->size += cur_size;
		return;
	}

	// end doesn't connect, makes new end
	block_t *end_new = (block_t *) ptr;
	end_new->size = cur_size-META_SIZE;
	end_new->prev = end;
	end_new->next = NULL;
	end_new->allocated = 0;
	total_blocks+=1;

	end->next = end_new;
	return;
}

// called when a block is freed, checks its neighbors to see if they can conjoin
void check_free(block_t *block) {
	if(block->prev) {
		block_t *prev = block->prev;

		// if prev and block are connected
		if(((char *) prev + META_SIZE + prev->size) == (char *) block && prev->allocated == 0) {
			prev->size += META_SIZE + block->size;
			prev->next = block->next;
			if(block->next) {
				prev->next->prev = prev;
			}
			block = prev;
			total_blocks -= 1;
		}
	}

	// checking block and next block
	if(block->next) {
		block_t *next = block->next;

		// if prev and block are connected
		if(((char *) block + META_SIZE + block->size) == (char *) next && next->allocated == 0) {
			block->size += META_SIZE + next->size;
			block->next = next->next;

			if(next->next) {
				next->next->prev = block;
			}
			total_blocks -= 1;
		}
	}
}

// BUDDY FUNCTIONS

uint check_allocate_buddy(block_t *block, size_t size) {
	if(block->size < size || block->allocated == 1) return INT_MAX;

	uint count = 1;
	size_t cur_block_size = block->size + META_SIZE;
	
	// Safety check: prevent infinite loop and overflow
	uint max_count = 32;
	
	while(cur_block_size / 2 >= (size + META_SIZE) && count < max_count) {
		cur_block_size /= 2;
		count++;
	}
	return count;
}

void break_block(block_t *block, int break_number) {
	if(break_number <= 1 || !block) return;  // Safety check
	
	// Find which mmap region this block belongs to
	int ptr_index = -1;
	for(int i = 0; i < ptr_counter; i++) {
		if((char *) block >= (char *) mmap_ptr[i] && (char *) block < (char *) mmap_ptr[i] + mmap_regions[i]) {
			ptr_index = i;
			break;
		}
	}
	if(ptr_index < 0) return;  // Block not in any mmap region
	
	size_t total_size = META_SIZE + block->size;
	int current = 1;
	while (current < break_number) {
		// makes new free block (second half)
		total_size /= 2;
		//fprintf(stderr, "breaking block, new size %lu \n", total_size);
		block_t *old_next = block->next;
		char *candidate = ((char *) block + total_size);
		
		// makes sure new_block stays within the same mmap region
		if(candidate < (char *) mmap_ptr[ptr_index] || candidate >= (char *) mmap_ptr[ptr_index] + mmap_regions[ptr_index]) {
			return;  // Cannot split further without exceeding region bounds
		}

		block_t *new_block = (block_t *)candidate;

		if(!new_block) return;  // Prevent invalid memory access
		
		new_block->size = total_size - META_SIZE;
		new_block->allocated = 0;
		new_block->requested = 0;
		new_block->next = old_next;
		new_block->prev = block;
		
		total_blocks += 1; 

		// updates neighbor
		if(old_next) {
			old_next->prev = new_block;
		}

		//updates original block
		block->size = total_size - META_SIZE;
		block->next = new_block;

		current++;
	}
	
}

void free_check_buddy(block_t *block) {
	if(!block) return;
	int prev_alloc = block->prev ? block->prev->allocated : 1;
	int next_alloc = block->next ? block->next->allocated : 1;
	if(prev_alloc == 1 && next_alloc == 1) {
		return;
	}

	int ptr_index = -1;
	for(int i = 0; i < ptr_counter; i++) {
		if((char *) block >= (char *) mmap_ptr[i] && (char *) block < (char *) mmap_ptr[i] + mmap_regions[i]) {
			ptr_index = i;
			break;
		}
	}
	if(ptr_index == -1) {
		ptr_index = ptr_counter - 1;
	}

	// Safety check: ensure ptr_index is valid
	if(ptr_index < 0 || ptr_index >= ptr_counter) {
		return;
	}

	size_t block_size = block->size + META_SIZE;
	uintptr_t block_address = (char *) block - (char *) mmap_ptr[ptr_index];
	uintptr_t buddy_address = block_address ^ block_size;
	
	// Validate buddy address is within the same mmap'd region as the block
	// Calculate potential buddy location and verify it's in the same region
	block_t *buddy = (block_t *) ((char *)mmap_ptr[ptr_index] + buddy_address);
	
	// Check if buddy is within the same mmap region
	if((char *) buddy < (char *) mmap_ptr[ptr_index] || (char *) buddy >= (char *) mmap_ptr[ptr_index] + mmap_regions[ptr_index]) {
		return; // Buddy is out of bounds, cannot merge
	}

	// to merge, before must be free and have the correct buddy address
	if((block->prev == buddy || block->next == buddy) &&
		buddy->allocated == 0 && (buddy->size + META_SIZE) == block_size) {
		
		if(block->prev == buddy) {
			buddy->size += block_size;
			buddy->next = block->next;
			if(block->next) {
				block->next->prev = buddy;
			}
			block = buddy;
			total_blocks -= 1;
			free_check_buddy(block);
			return;
		} else if (block->next == buddy) {
			block->size += block_size;
			block->next = block->next->next;
			if(block->next) {
				block->next->prev = block;
			}
			total_blocks -= 1;
			free_check_buddy(block);
		}
	}
	return;
}

// calls mmap and adds to end
void expand_buddy(block_t *end, size_t size) {
	if(end == NULL) return;
	// TO DO : if end is free, check if mmap returns address that connects
	// if end if allocated, just add on
	size_t cur_size = 4096;
	while(cur_size < size + META_SIZE) {
		cur_size *= 2;
	}

	void *ptr = mmap(NULL, cur_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED) {
		fprintf(stderr, "mmap failed");
		exit(1);
	}
	bytes_requested += cur_size;
	mmap_ptr[ptr_counter] = ptr;
	mmap_regions[ptr_counter] = cur_size;
	ptr_counter++;

	// end doesn't connect, makes new end
	block_t *end_new = (block_t *) ptr;
	end_new->size = cur_size-META_SIZE;
	end_new->prev = end;
	end_new->next = NULL;
	end_new->allocated = 0;
	end_new->requested = 0;
	total_blocks+=1;

	end->next = end_new;
	return;
}

void t_init(alloc_strat_e strat) {
	bytes_requested = 4096;
	total_blocks = 1;
	// TODO: Implement this
	alloc_strat = strat;
	if(alloc_strat == MIXED) {
		alloc_strat = FIRST_FIT;
		mixed = 1;
	}
	void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED) {
		fprintf(stderr, "mmap failed");
		exit(1);
	}

	mmap_ptr[0] = ptr;
	mmap_regions[0] = 4096;
	ptr_counter = 1;
	heap_head = (block_t *) ptr;
	heap_head->size = 4096-META_SIZE;
	heap_head->prev = NULL;
	heap_head->next = NULL;
	heap_head->allocated = 0;
	heap_head->requested = 0;
}

void *t_malloc(size_t size) {
	if(heap_head == NULL) {
		fprintf(stderr, "Allocator not instantialized");
		exit(1);
	}

	if(size % 4 != 0) {
		size += (4 - size % 4);
	}

	if(mixed == 1) {
		if(alloc_strat == FIRST_FIT) {
			alloc_strat = BEST_FIT;
		} else if (alloc_strat == BEST_FIT) {
			alloc_strat = WORST_FIT;
		} else {
			alloc_strat = FIRST_FIT;
		}
	}

	//fprintf(stderr, "\nmalloc has been called, requested size = %lu", size);

	if(alloc_strat == BUDDY) {
		block_t *current = heap_head;
		block_t *second_best = NULL;
		int second_size_mult = INT_MAX;
		uint size_mult;
		while(current) {
			size_mult = check_allocate_buddy(current, size);
			if(size_mult == 1) {
				current->allocated = 1;
				current->requested = (uint) size;
				//fprintf(stderr, "requested updated, %u", current->requested);
				return (char *) current + META_SIZE;
			} else if (size_mult < second_size_mult) {
				second_size_mult = size_mult;
				second_best = current;
			}
			if(current->next == NULL) break;
			current = current->next;
		}
		if(second_best) {
			//fprintf(stderr, "break block has been called at second_best, second size mult : %u", second_size_mult);
			break_block(second_best, second_size_mult);
			second_best->allocated = 1;
			second_best->requested = (uint) size;
			//fprintf(stderr, "requested updated, %u", second_best->requested);
			return (char *) second_best + META_SIZE;
		}
		//fprintf(stderr, "calling expand buddy");
		expand_buddy(current, size);
		if(current->next) {
			size_mult = check_allocate_buddy(current->next, size);
			break_block(current->next, size_mult);
			current->next->allocated = 1;
			current->next->requested = (uint) size;
			//fprintf(stderr, "requested updated, %u\n", current->next->requested);
			return (char *) current->next + META_SIZE;
		}
	
	} else if(alloc_strat == FIRST_FIT) {
		block_t *current = heap_head;
		while(current) {
			if(check_allocate(current, size)) {
				allocate(current, size);
				return (char *) current + META_SIZE;
			}
			if(current->next == NULL) break;
			current = current->next;
		}
		expand(current, size);
		if(current->next) {
			allocate(current->next, size);
			return (char *) current->next + META_SIZE;

		} else {
			allocate(current, size);
			return (char *) current + META_SIZE;
		}

	} else if(alloc_strat == BEST_FIT) {
		block_t *best = NULL;
		block_t *current = heap_head;

		// traverses through list
		while(current) {
			if(check_allocate(current, size)) {
				if(best) {
					if(best->size > current->size) {
						best = current;
					}
				} else {
					best = current;
				}
			}
			if(current->next == NULL) break;
			current = current->next;
		}

		// checks if block was found
		if(best) {
			allocate(best, size);
			return (char *) best + META_SIZE;
		} else {
			expand(current, size);
			if(current->next) {
				allocate(current->next, size);
				return (char *) current->next + META_SIZE;

			} else {
				allocate(current, size);
				return (char *) current + META_SIZE;
			}
		}

	} else if(alloc_strat == WORST_FIT) {
		block_t *worst = NULL;
		block_t *current = heap_head;

		// traverses through list
		while(current) {
			if(check_allocate(current, size)) {
				if(worst) {
					if(worst->size < current->size) {
						worst = current;
					}
				} else {
					worst = current;
				}
			}
			if(current->next == NULL) break;
			current = current->next;
		}

		// checks if block was found
		if(worst) {
			allocate(worst, size);
			return (char *) worst + META_SIZE;
		} else {
			expand(current, size);
			if(current->next) {
				allocate(current->next, size);
				return (char *) current->next + META_SIZE;

			} else {
				allocate(current, size);
				return (char *) current + META_SIZE;
			}
		}

	} else {
		fprintf(stderr, "error: stategy not implemented yet");
		exit(1);
	}
	return NULL;
}

void t_free(void *ptr) {
	if(ptr == NULL) return;

	block_t *block = (block_t *) ((char *) ptr - META_SIZE);
	if(!block) return;
	block->allocated = 0;
	if(alloc_strat == BUDDY) {
		block->requested = 0;
		free_check_buddy(block);
	} else {
		check_free(block);
	}
	return;
}

// METHOD USED FOR RESULTS
double get_mem_util() {
	uint mem_allocated = 0;

	block_t *current = heap_head;
	while(current) {
		if(current->allocated == 1) {
			mem_allocated += current->size;
		}
		if(current->next == NULL) break;
		current = current->next;
	}
	double percent = (double) mem_allocated / bytes_requested;
	return percent;
}

double get_mem_util_buddy() {
	uint mem_allocated = 0;

	block_t *current = heap_head;
	while(current) {
		if(current->allocated == 1) {
			//fprintf(stderr, "smth is allocated, requested is %u,", current->requested);
			mem_allocated += current->requested;
		}
		if(current->next == NULL) break;
		current = current->next;
	}
	//fprintf(stderr, "\n done: mem allocated is %u, bytes requested is %lu\n", mem_allocated, bytes_requested);
	double percent = ((double) mem_allocated) / bytes_requested;
	return percent;
}


uint get_overhead() {
	return total_blocks * META_SIZE;
}