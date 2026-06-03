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
    graph_add_node(g, 0, 0.0, 0.0); // Hospital
    graph_add_node(g, 1, 0.0, 2.0);
    graph_add_node(g, 2, 3.0, 2.0);
    graph_add_node(g, 3, 0.0, 5.0);
    graph_add_node(g, 4, 5.0, 0.0);
    graph_add_node(g, 5, 3.0, 5.0);
    graph_add_node(g, 6, 1.0, 7.0);
    graph_add_node(g, 7, 4.0, 7.0); // Fire Station

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

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC;
}

static void experiment_density(void) {
    printf("\n=== EXPERIMENT 2: Effect of Graph Density (20x20 grid) ===\n");
    printf("Routing from node 0 to node 399\n\n");

    double densities[] = {0.3, 0.6, 1.0};
    const char *labels[] = {"Sparse (0.3)", "Medium (0.6)", "Dense  (1.0)"};
    int rows = 20, cols = 20;

    for (int d = 0; d < 3; d++) {
        Graph *g = graph_generate_grid(rows, cols, densities[d], 1, 42 + d);
        if (!g) continue;

        int src = graph_find_node(g, 0);
        int dst = graph_find_node(g, rows * cols - 1);

        printf("  [%s]  nodes=%d  edges=%d\n",
               labels[d], g->num_nodes, g->num_edges);

        clock_t t0 = clock();
        DijkstraResult *dr = dijkstra_run(g, src);
        clock_t t1 = clock();

        clock_t t2 = clock();
        AStarResult *ar = astar_run(g, src, dst, 1.0);
        clock_t t3 = clock();

        printf("  %-20s  visited=%-6d  dist=%.4f  time=%.6fs\n",
               "Dijkstra",
               dr->nodes_visited, dr->dist[dst],
               elapsed_seconds(t0, t1));
        printf("  %-20s  visited=%-6d  dist=%.4f  time=%.6fs\n",
               "A* (w=1.0)",
               ar->nodes_visited, ar->path_cost,
               elapsed_seconds(t2, t3));

        if (dr->nodes_visited > 0) {
            double ratio = 100.0 * ar->nodes_visited / dr->nodes_visited;
            printf("  A* visited %.1f%% of nodes Dijkstra visited\n", ratio);
        }
        printf("\n");

        dijkstra_result_free(dr);
        astar_result_free(ar);
        graph_destroy(g);
    }
}

static void experiment_size(void) {
    printf("\n=== EXPERIMENT 3: Scalability — Effect of Graph Size ===\n");
    printf("Density fixed at 0.6.  Routing corner to corner.\n\n");
    printf("  %-12s  %-8s  %-8s  %-10s  %-10s  %-10s  %-10s\n",
           "Grid", "Nodes", "Edges",
           "Dijk.Vis", "A*.Vis",
           "Dijk.t(s)", "A*.t(s)");
    printf("  ------------------------------------------------------------\n");

    int sizes[][2] = {{5,5},{10,10},{20,20},{30,30}};
    int n_sizes = 4;

    for (int s = 0; s < n_sizes; s++) {
        int rows = sizes[s][0], cols = sizes[s][1];
        Graph *g = graph_generate_grid(rows, cols, 0.6, 1, 100 + s);
        if (!g) continue;

        int src = graph_find_node(g, 0);
        int dst = graph_find_node(g, rows * cols - 1);

        clock_t t0 = clock();
        DijkstraResult *dr = dijkstra_run(g, src);
        clock_t t1 = clock();

        clock_t t2 = clock();
        AStarResult *ar = astar_run(g, src, dst, 1.0);
        clock_t t3 = clock();

        char label[16];
        snprintf(label, sizeof(label), "%dx%d", rows, cols);
        printf("  %-12s  %-8d  %-8d  %-10d  %-10d  %-10.6f  %-10.6f\n",
               label, g->num_nodes, g->num_edges,
               dr->nodes_visited, ar->nodes_visited,
               elapsed_seconds(t0, t1), elapsed_seconds(t2, t3));

        dijkstra_result_free(dr);
        astar_result_free(ar);
        graph_destroy(g);
    }
    printf("\n");
}

static void experiment_heuristic_weight(void) {
    printf("\n=== EXPERIMENT 4: A* Heuristic Weight (20x20, density=0.6) ===\n");
    printf("w=1.0 => optimal;  w>1.0 => faster but possibly suboptimal\n\n");

    Graph *g = graph_generate_grid(20, 20, 0.6, 1, 999);
    if (!g) return;

    int src = graph_find_node(g, 0);
    int dst = graph_find_node(g, 399);

    clock_t t0 = clock();
    DijkstraResult *dr = dijkstra_run(g, src);
    clock_t t1 = clock();
    double opt_dist = dr->dist[dst];
    printf("  Dijkstra (optimal):  dist=%.4f  time=%.6fs\n\n",
           opt_dist, elapsed_seconds(t0, t1));
    dijkstra_result_free(dr);

    printf("  %-8s  %-8s  %-12s  %-10s  %-10s\n",
           "Weight", "Visited", "Dist", "Err%", "Time(s)");
    printf("  ------------------------------------------------\n");

    double weights[] = {1.0, 1.5, 2.0, 3.0};
    for (int w = 0; w < 4; w++) {
        clock_t ta = clock();
        AStarResult *ar = astar_run(g, src, dst, weights[w]);
        clock_t tb = clock();

        double dist = ar->path_cost;
        double err  = 100.0 * (dist - opt_dist) / opt_dist;
        printf("  %-8.1f  %-8d  %-12.4f  %-10.2f  %-10.6f\n",
               weights[w], ar->nodes_visited, dist, err,
               elapsed_seconds(ta, tb));
        astar_result_free(ar);
    }
    printf("\n");
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
        experiment_density();
        experiment_size();
        experiment_heuristic_weight();
        separator();
        printf("All experiments complete.\n");
        separator();
    }

    return 0;
}