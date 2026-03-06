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

void test_strategies(alloc_strat_e strat, int type) {
    printf("Running test_strategy %u...\n", type);

    t_init(strat);

    void *p1 = t_malloc(200);
    void *expected_first = t_malloc(400);
    void *p2 = t_malloc(200);
    void *h1 = t_malloc(800);
    void *p3 = t_malloc(200);
    void *expected_best = t_malloc(200);
    void *p4 = t_malloc(200);
    void *expected_worst = t_malloc(100);

    t_free(expected_first);
    t_free(h1);
    t_free(expected_best);
    t_free(expected_worst);

    void *actual = t_malloc(100);

    // FIRST
    if(type == 1) {
        assert(actual == expected_first);
        void *actual2 = t_malloc(750);
        assert(actual2 == h1); 
        void *actual3 = t_malloc(300); 
        assert(actual3 == expected_worst);
    } else if (type == 2) { // BEST
        assert(actual == expected_best);
        void *actual2 = t_malloc(900); 
        assert(actual2 == expected_worst); 
        void *actual3 = t_malloc(200); 
        assert(actual3 == expected_first);
    } else if (type == 3) { // WORST
        assert(actual == expected_worst);
    } else { // mixed
        assert(actual == expected_worst); // worst
        void *actual2 = t_malloc(300); // first
        assert(actual2 == expected_first); 
        void *actual3 = t_malloc(400); // best
        assert(actual3 == h1);
    }

    printf("Passed\n\n");
}

// checks for seg faulting
void test_buddy() {
    printf("Running test_strategy_buddy...\n");

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
    test_strategies(FIRST_FIT, 1);
    test_strategies(BEST_FIT, 2);
    test_strategies(WORST_FIT, 3);
    test_strategies(MIXED, 4);
    test_buddy();

    printf("All tests passed.\n\n");

    return 0;
}