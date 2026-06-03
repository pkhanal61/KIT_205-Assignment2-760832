#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "graph.h"
#include "dijkstra.h"
#include "astar.h"
#include "test.h"

static void separator(void) {
    printf("----------------------------------------------------------\n");
}


int main(int argc, char *argv[]) {
    printf("=========================================================\n");
    printf("  KIT205 Assignment 2 — Emergency Vehicle Routing\n");
    printf("  Dijkstra vs A* on Road Network Graphs\n");
    printf("=========================================================\n");

    int run_tests = 1;
    int run_exp   = 1;

    if (argc >= 2) {
        if (strcmp(argv[1], "tests") == 0) { run_exp   = 0; }
        if (strcmp(argv[1], "exp")   == 0) { run_tests = 0; }
    }

    if (run_tests) {
        separator();
        printf("PART 1 — UNIT TESTS\n");
        separator();

        int total_run = 0, total_pass = 0, total_fail = 0;

        test_reset(); test_graph();
        total_run  += tests_run;
        total_pass += tests_passed;
        total_fail += tests_failed;

        test_reset(); test_priority_queue();
        total_run  += tests_run;
        total_pass += tests_passed;
        total_fail += tests_failed;

        test_reset(); test_dijkstra();
        total_run  += tests_run;
        total_pass += tests_passed;
        total_fail += tests_failed;

        test_reset(); test_astar();
        total_run  += tests_run;
        total_pass += tests_passed;
        total_fail += tests_failed;

        separator();
        printf("TOTAL: %d/%d tests passed", total_pass, total_run);
        if (total_fail > 0)
            printf("  *** %d FAILED ***", total_fail);
        printf("\n");
        separator();
    }

    return 0;
}