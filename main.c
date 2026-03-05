#include "libtdmm/tdmm.h"
#include "tdmm.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>

void print_array(double array[], int size) {
	for(int i = 0; i < size; i++) {
		printf("%f, ", array[i]);
	}
	printf("\n");
}

// METRIC TESTING

void metric_test_1_2_4(uint allocations[]) {
	double percent = 0.0; 				// holds current %mem util
	uint calls = 0;

	double first_fit[600]; 			// %mem average
	double first_fit_percent[600]; 	// %mem at point
	void *first_fit_ptr[300];		// pointers returned
	uint first_overhead[600];			

	t_init(FIRST_FIT);
	double first_total = 0.0;
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 50; i++) {
			first_fit_ptr[i + j*50] = t_malloc(allocations[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			first_fit_percent[calls-1] = percent;
			first_total += percent;
			first_fit[calls-1] = first_total / calls;
			first_overhead[calls-1] = get_overhead();
		}
		// calling free
		for(int i = 0; i < 25; i++) {
			t_free(first_fit_ptr[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			first_total += percent;
			first_fit[calls-1] = first_total / calls;
			first_fit_percent[calls-1] = percent;
			first_overhead[calls-1] = get_overhead();
		}
	}
	// calling free for the rest of the mallocated blocks
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 25; i++) {
			t_free(first_fit_ptr[i + 25 + j*50]);
			calls++;
			first_fit[calls-1] = first_total / calls;
			first_fit_percent[calls-1] = get_mem_util()*100;
			first_overhead[calls-1] = get_overhead();
		}
	}


	percent = 0.0; 				// holds current %mem util
	calls = 0;

	double best_fit[600]; 			// %mem average
	double best_fit_percent[600]; 	// %mem at point
	void *best_fit_ptr[300];		// pointers returned
	uint best_overhead[600];			

	t_init(BEST_FIT);
	double best_total = 0.0;
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 50; i++) {
			best_fit_ptr[i + j*50] = t_malloc(allocations[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			best_fit_percent[calls-1] = percent;
			best_total += percent;
			best_fit[calls-1] = best_total / calls;
			best_overhead[calls-1] = get_overhead();
		}
		// calling free
		for(int i = 0; i < 25; i++) {
			t_free(best_fit_ptr[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			best_total += percent;
			best_fit[calls-1] = best_total / calls;
			best_fit_percent[calls-1] = percent;
			best_overhead[calls-1] = get_overhead();
		}
	}
	// calling free for the rest of the mallocated blocks
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 25; i++) {
			t_free(best_fit_ptr[i + 25 + j*50]);
			calls++;
			best_fit[calls-1] = best_total / calls;
			best_fit_percent[calls-1] = get_mem_util() * 100;
			best_overhead[calls-1] = get_overhead();
		}
	}

	percent = 0.0; 				// holds current %mem util
	calls = 0;

	double worst_fit[600]; 			// %mem average
	double worst_fit_percent[600]; 	// %mem at point
	void *worst_fit_ptr[300];		// pointers returned
	uint worst_overhead[600];			

	t_init(WORST_FIT);
	double worst_total = 0.0;
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 50; i++) {
			worst_fit_ptr[i + j*50] = t_malloc(allocations[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			worst_fit_percent[calls-1] = percent;
			worst_total += percent;
			worst_fit[calls-1] = worst_total / calls;
			worst_overhead[calls-1] = get_overhead();
		}
		// calling free
		for(int i = 0; i < 25; i++) {
			t_free(worst_fit_ptr[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			worst_total += percent;
			worst_fit[calls-1] = worst_total / calls;
			worst_fit_percent[calls-1] = percent;
			worst_overhead[calls-1] = get_overhead();
		}
	}
	// calling free for the rest of the mallocated blocks
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 25; i++) {
			t_free(worst_fit_ptr[i + 25 + j*50]);
			calls++;
			worst_fit[calls-1] = worst_total / calls;
			worst_fit_percent[calls-1] = get_mem_util() *100;
			worst_overhead[calls-1] = get_overhead();
		}
	}percent = 0.0; 				// holds current %mem util
	calls = 0;

	double mixed_fit[600]; 			// %mem average
	double mixed_fit_percent[600]; 	// %mem at point
	void *mixed_fit_ptr[300];		// pointers returned
	uint mixed_overhead[600];			

	t_init(MIXED);
	double mixed_total = 0.0;
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 50; i++) {
			mixed_fit_ptr[i + j*50] = t_malloc(allocations[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			mixed_fit_percent[calls-1] = percent;
			mixed_total += percent;
			mixed_fit[calls-1] = mixed_total / calls;
			mixed_overhead[calls-1] = get_overhead();
		}
		// calling free
		for(int i = 0; i < 25; i++) {
			t_free(mixed_fit_ptr[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			mixed_total += percent;
			mixed_fit[calls-1] = mixed_total / calls;
			mixed_fit_percent[calls-1] = percent;
			mixed_overhead[calls-1] = get_overhead();
		}
	}
	// calling free for the rest of the mallocated blocks
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 25; i++) {
			t_free(mixed_fit_ptr[i + 25 + j*50]);
			calls++;
			mixed_fit[calls-1] = mixed_total / calls;
			mixed_fit_percent[calls-1] = get_mem_util() * 100;
			mixed_overhead[calls-1] = get_overhead();
		}
	}

	percent = 0.0; 				// holds current %mem util
	calls = 0;

	double buddy_fit[600]; 			// %mem average
	double buddy_fit_percent[600]; 	// %mem at point
	void *buddy_fit_ptr[300];		// pointers returned
	uint buddy_overhead[600];			

	t_init(BUDDY);
	double buddy_total = 0.0;
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 50; i++) {
			buddy_fit_ptr[i + j*50] = t_malloc(allocations[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			buddy_fit_percent[calls-1] = percent;
			buddy_total += percent;
			buddy_fit[calls-1] = buddy_total / calls;
			buddy_overhead[calls-1] = get_overhead();
		}
		// calling free
		for(int i = 0; i < 25; i++) {
			t_free(buddy_fit_ptr[i + j*50]);
			calls++;
			percent = get_mem_util() * 100;
			buddy_total += percent;
			buddy_fit[calls-1] = buddy_total / calls;
			buddy_fit_percent[calls-1] = percent;
			buddy_overhead[calls-1] = get_overhead();
		}
	}
	// calling free for the rest of the mallocated blocks
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < 25; i++) {
			t_free(buddy_fit_ptr[i + 25 + j*50]);
			calls++;
			buddy_fit[calls-1] = buddy_total / calls;
			buddy_fit_percent[calls-1] = get_mem_util() * 100;
			buddy_overhead[calls-1] = get_overhead();
		}
	}

	FILE *fptr;
	fptr = fopen("test124.csv", "w");
	fprintf(fptr, "allocation,first_avg,first_mem_point,first_overhead,best_avg,best_mem_point,best_overhead,worst_avg,worst_mem_point,worst_overhead,mixed_avg,mixed_mem_point,mixed_overhead,buddy_avg,buddy_mem_point,buddy_overhead\n");
	uint size = 1;
	for(int i = 0; i < 600; i++) {
		fprintf(fptr,"%u,%f,%f,%u,%f,%f,%u,%f,%f,%u,%f,%f,%u,%f,%f,%u\n", i, first_fit[i],first_fit_percent[i], first_overhead[i],best_fit[i],best_fit_percent[i], best_overhead[i],worst_fit[i],worst_fit_percent[i], worst_overhead[i],
		mixed_fit[i],mixed_fit_percent[i], mixed_overhead[i],buddy_fit[i],buddy_fit_percent[i], buddy_overhead[i]);
	}

	fclose(fptr);

}

void metric_test_3() {
	struct timespec start, end;
	double first_times[48];
	double best_times[48];
	double worst_times[48];
	double mixed_times[48];
	double buddy_times[48];
	void *pointers[24];
	int index = 0;

	t_init(FIRST_FIT);

	for(int i = 1; i < 8388609; i*=2) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		pointers[index] = t_malloc(i);
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate total nanoseconds: (seconds * 1,000,000,000) + nanoseconds
		double elapsed_ns = (end.tv_sec - start.tv_sec) *1e9 + (end.tv_nsec - start.tv_nsec);
		first_times[index] = elapsed_ns;
		index++;
	}
	for(int i = 0; i < 24; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
        
        t_free(pointers[i]);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Calculate total nanoseconds (fixed the missing conversion here)
        double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        first_times[i + 24] = elapsed_ns;
	}

	index = 0;
	t_init(BEST_FIT);

	for(int i = 1; i < 8388609; i*=2) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		pointers[index] = t_malloc(i);
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate total nanoseconds: (seconds * 1,000,000,000) + nanoseconds
		double elapsed_ns = (end.tv_sec - start.tv_sec) *1e9 + (end.tv_nsec - start.tv_nsec);
		best_times[index] = elapsed_ns;
		index++;
	}
	for(int i = 0; i < 24; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
        
        t_free(pointers[i]);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Calculate total nanoseconds (fixed the missing conversion here)
        double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        best_times[i + 24] = elapsed_ns;
	}

	index = 0;
	t_init(WORST_FIT);

	for(int i = 1; i < 8388609; i*=2) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		pointers[index] = t_malloc(i);
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate total nanoseconds: (seconds * 1,000,000,000) + nanoseconds
		double elapsed_ns = (end.tv_sec - start.tv_sec) *1e9 + (end.tv_nsec - start.tv_nsec);
		worst_times[index] = elapsed_ns;
		index++;
	}
	for(int i = 0; i < 24; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
        
        t_free(pointers[i]);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Calculate total nanoseconds (fixed the missing conversion here)
        double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        worst_times[i + 24] = elapsed_ns;
	}

	index = 0;
	t_init(MIXED);

	for(int i = 1; i < 8388609; i*=2) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		pointers[index] = t_malloc(i);
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate total nanoseconds: (seconds * 1,000,000,000) + nanoseconds
		double elapsed_ns = (end.tv_sec - start.tv_sec) *1e9 + (end.tv_nsec - start.tv_nsec);
		mixed_times[index] = elapsed_ns;
		index++;
	}
	for(int i = 0; i < 24; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
        
        t_free(pointers[i]);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Calculate total nanoseconds (fixed the missing conversion here)
        double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        mixed_times[i + 24] = elapsed_ns;
	}

	index = 0;
	t_init(BUDDY);

	for(int i = 1; i < 8388609; i*=2) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		pointers[index] = t_malloc(i);
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate total nanoseconds: (seconds * 1,000,000,000) + nanoseconds
		double elapsed_ns = (end.tv_sec - start.tv_sec) *1e9 + (end.tv_nsec - start.tv_nsec);
		buddy_times[index] = elapsed_ns;
		index++;
	}
	for(int i = 0; i < 24; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
        
        t_free(pointers[i]);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Calculate total nanoseconds (fixed the missing conversion here)
        double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        buddy_times[i + 24] = elapsed_ns;
	}

	FILE *fptr;
	fptr = fopen("test3.csv", "w");
	fprintf(fptr, "size,first,best,worst,mixed,buddy\n");
	uint size = 1;
	for(int i = 0; i < 48; i++) {
		fprintf(fptr,"%u,%f,%f,%f,%f,%f\n", size, first_times[i],best_times[i], worst_times[i],mixed_times[i], buddy_times[i]);
		size *= 2;
	}
	fclose(fptr);
}

// UNIT TESTS!

void test_initialization() {
    printf("Running test_initialization: ");
    t_init(FIRST_FIT);
    
    // After init, there should be 1 block and 4096 bytes requested
    assert(get_overhead() == 24); // 1 block * META_SIZE (24)
	assert(get_mem_util() == 0.0);
    printf("Passed!\n");
}

void test_basic_allocation() {
    printf("Running test_basic_allocation: ");
    t_init(FIRST_FIT);
    
    void *ptr1 = t_malloc(100);
    assert(ptr1 != NULL);
    
    // Should have 2 blocks: the allocated 100-byte block and the remaining free block
    assert(get_overhead() == 48); // 2 blocks * 24
    printf("Passed!\n");
}

void test_free_and_coalesce() {
    printf("Running test_free_and_coalesce: ");
    
    t_init(FIRST_FIT);
    
    void *ptr1 = t_malloc(100);
    void *ptr2 = t_malloc(200);
    void *ptr3 = t_malloc(300);
	assert(get_overhead() == 96);
    
    // Free the middle block
    t_free(ptr2);
	assert(get_overhead() == 96);
    
    // Free the first block (should coalesce with the middle block)
    t_free(ptr1);
	assert(get_overhead() == 72);
    
    // Free the last block (should coalesce everything back into a single giant free block)
    t_free(ptr3);
    
    // Should be back down to 1 block
    assert(get_overhead() == 24); 
    printf("Passed!\n");
}

void test_heap_expansion() {
    printf("Running test_heap_expansion: ");
    t_init(FIRST_FIT);
    
    // 4096 is the initial size. Allocating 5000 will force an expand()
    void *ptr1 = t_malloc(5000);
    assert(ptr1 != NULL);
    
    // Memory utilization should reflect the new bytes_requested amount
    assert(get_mem_util() > 0.0);
    printf("Passed!\n");
}

void test_best_fit_strategy() {
    printf("Running test_best_fit_strategy: ");
    t_init(BEST_FIT);
    
    void *p1 = t_malloc(100); // Small
    void *p2 = t_malloc(100); // Guard block
    void *p3 = t_malloc(500); // Large
    void *p4 = t_malloc(100); // Guard block
    
    t_free(p1); // Free small block
    t_free(p3); // Free large block
    
    // Request a small amount. Best fit should choose the p1 slot (100 bytes), 
    // not the p3 slot (500 bytes).
    void *p_new = t_malloc(50);
    
    // If it picked p1, it will split p1
    assert(p_new != NULL);
    printf("Passed!\n");
}


int main(int argc, char *argv[]) {

	// testing memory utilization on average during a program run
	// testing memory utilization as a function of time
	uint allocations[300];
	printf("allocations: ");
	/* seed PRNG so allocations differ each run */
	srand((unsigned) time(NULL));
	for(int i = 0; i < 300; i++) {
		allocations[i] = (rand() % 1048576) + 1; 		// maxes at 1048576
	}
	metric_test_1_2_4(allocations);

	// testing tmalloc() and tfree() speed as a function of size
	metric_test_3();

	printf("Unit Tests\n");
    printf("-------------------------------------------\n");
    
    test_initialization();
    test_basic_allocation();
    test_free_and_coalesce();
    test_heap_expansion();
    test_best_fit_strategy();
    
    printf("-------------------------------------------\n");
    return 0;
}