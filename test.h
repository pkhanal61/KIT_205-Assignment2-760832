#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <math.h>

// Global counters
extern int tests_run;
extern int tests_passed;
extern int tests_failed;

// Core assertion macro 
#define ASSERT(desc, condition)                                    \
    do {                                                           \
        tests_run++;                                               \
        if (condition) {                                           \
            printf("  [PASS] %s\n", desc);                        \
            tests_passed++;                                        \
        } else {                                                   \
            printf("  [FAIL] %s  (line %d)\n", desc, __LINE__);   \
            tests_failed++;                                        \
        }                                                          \
    } while (0)

// Floating point equality with tolerance 
#define ASSERT_NEAR(desc, a, b, tol) \
    ASSERT(desc, fabs((a) - (b)) < (tol))

void test_reset(void);
void test_summary(void);

// Test suite declarations
void test_graph(void);
void test_priority_queue(void);
void test_dijkstra(void);
void test_astar(void);

#endif