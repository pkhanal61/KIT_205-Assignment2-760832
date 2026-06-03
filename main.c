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
static void experiment_city_demo(void) {
    printf("\n=== EXPERIMENT 1: Small City Road Network Demo ===\n");
    printf("8 intersections, emergency vehicle from Hospital (0) to Fire Station (7)\n\n");

    /*
     * Layout (coordinates in km):
     *   0(Hospital) -2-> 1 -3-> 2 -2-> 5 -1-> 7(Fire Station)
     *   0 -4-> 3 -3-> 6 -2-> 7
     *   0 -5-> 4 -4-> 5
     */
    Graph *g = graph_create(1);
    graph_add_node(g, 0, 0.0, 0.0); /* Hospital     */
    graph_add_node(g, 1, 0.0, 2.0);
    graph_add_node(g, 2, 3.0, 2.0);
    graph_add_node(g, 3, 0.0, 5.0);
    graph_add_node(g, 4, 5.0, 0.0);
    graph_add_node(g, 5, 3.0, 5.0);
    graph_add_node(g, 6, 1.0, 7.0);
    graph_add_node(g, 7, 4.0, 7.0); /* Fire Station */

    graph_add_edge(g, 0, 1, 2.0);
    graph_add_edge(g, 0, 4, 5.0);
    graph_add_edge(g, 1, 2, 3.0);
    graph_add_edge(g, 1, 3, 4.0);
    graph_add_edge(g, 2, 5, 2.0);
    graph_add_edge(g, 3, 6, 3.0);
    graph_add_edge(g, 4, 5, 4.0);
    graph_add_edge(g, 5, 7, 1.0);
    graph_add_edge(g, 6, 7, 2.0);

    int src = graph_find_node(g, 0);
    int dst = graph_find_node(g, 7);

    DijkstraResult *dr = dijkstra_run(g, src);
    printf("  Dijkstra: ");
    dijkstra_print_path(g, dr, src, dst);

    AStarResult *ar1 = astar_run(g, src, dst, 1.0);
    printf("  A*(w=1):  ");
    astar_print_path(g, ar1, src, dst);

    AStarResult *ar2 = astar_run(g, src, dst, 2.0);
    printf("  A*(w=2):  ");
    astar_print_path(g, ar2, src, dst);

    dijkstra_result_free(dr);
    astar_result_free(ar1);
    astar_result_free(ar2);
    graph_destroy(g);
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

    if (run_exp) {
        printf("\nPART 2 — EXPERIMENTS\n");
        separator();
        experiment_city_demo();
        separator();
        printf("All experiments complete.\n");
        separator();
    }

    return 0;
}