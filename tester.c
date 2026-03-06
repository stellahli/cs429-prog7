#include "libtdmm/tdmm.h"
#include "tdmm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N 2000

void test_initialization() {
    printf("Running test_initialization...\n");

    t_init(FIRST_FIT);

    assert(get_overhead() == 32);
    assert(get_mem_util() == 0.0);

    printf("Passed\n\n");
}

void test_basic_allocation() {
    printf("Running test_basic_allocation...\n");

    t_init(FIRST_FIT);

    void *ptr = t_malloc(100);
    assert(ptr != NULL);

    assert(get_overhead() == 64);

    printf("Passed\n\n");
}

void test_free_and_coalesce() {
    printf("Running test_free_and_coalesce...\n");

    t_init(FIRST_FIT);

    void *p1 = t_malloc(100);
    void *p2 = t_malloc(200);
    void *p3 = t_malloc(300);

    assert(get_overhead() == 32*4);

    t_free(p2);
    assert(get_overhead() == 32*4);

    t_free(p1);
    assert(get_overhead() == 32*3);

    t_free(p3);
    assert(get_overhead() == 32);

    printf("Passed\n\n");
}

void test_heap_expansion() {
    printf("Running test_heap_expansion...\n");

    t_init(FIRST_FIT);

    void *ptr = t_malloc(5000);
    assert(ptr != NULL);

    assert(get_mem_util() > 0.0);

    printf("Passed\n\n");
}

void test_best_fit_strategy() {
    printf("Running test_best_fit_strategy...\n");

    t_init(BEST_FIT);

    void *p1 = t_malloc(100);
    void *p2 = t_malloc(100);
    void *p3 = t_malloc(500);
    void *p4 = t_malloc(100);

    t_free(p1);
    t_free(p3);

    void *p_new = t_malloc(50);
    assert(p_new != NULL);

    printf("Passed\n\n");
}

void test_buddy() {
    printf("Running test_buddy_allocator...\n");

    t_init(BUDDY);

    void *ptrs[N];

    for (int i = 0; i < N; i++) {

        size_t size;

        if (i % 4 == 0) size = 16;
        else if (i % 4 == 1) size = 64;
        else if (i % 4 == 2) size = 200;
        else size = 500;

        ptrs[i] = t_malloc(size);

        if (!ptrs[i]) {
            printf("Allocation failed at %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < N; i += 2) {
        t_free(ptrs[i]);
        ptrs[i] = NULL;
    }

    for (int i = 0; i < N; i += 2) {

        size_t size = (i % 3 == 0) ? 32 : 128;

        ptrs[i] = t_malloc(size);

        if (!ptrs[i]) {
            printf("Reallocation failed at %d\n", i);
            exit(1);
        }
    }

    for (int i = N - 1; i >= 0; i -= 3) {
        if (ptrs[i]) {
            t_free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

    for (int i = 0; i < N; i++) {
        if (!ptrs[i]) {
            ptrs[i] = t_malloc((i % 5 + 1) * 50);
        }
    }

    for (int i = 0; i < N; i++) {
        if (ptrs[i]) {
            t_free(ptrs[i]);
        }
    }

    printf("Passed\n\n");
}

int main() {

    printf("\nAllocator Tests\n\n");

    test_initialization();
    test_basic_allocation();
    test_free_and_coalesce();
    test_heap_expansion();
    test_best_fit_strategy();
    test_buddy();

    printf("All tests passed.\n\n");

    return 0;
}