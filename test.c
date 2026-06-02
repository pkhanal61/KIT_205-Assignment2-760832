#include "test.h"
#include "graph.h"
#include "priority_queue.h"
#include "dijkstra.h"
#include "astar.h"
#include <stdlib.h>
#include <math.h>

int tests_run    = 0;
int tests_passed = 0;
int tests_failed = 0;

void test_reset(void) {
    tests_run    = 0;
    tests_passed = 0;
    tests_failed = 0;
}

void test_summary(void) {
    printf("  Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
        printf("  (%d FAILED)", tests_failed);
    printf("\n");
}

void test_graph(void) {
    printf("\n=== UNIT TESTS: Graph Data Structure ===\n");

    // creation 
    Graph *g = graph_create(1);
    ASSERT("graph_create returns non-NULL", g != NULL);
    ASSERT("initial node count is 0", graph_node_count(g) == 0);
    ASSERT("initial edge count is 0", graph_edge_count(g) == 0);

    // add nodes
    int i0 = graph_add_node(g, 0, 0.0, 0.0);
    int i1 = graph_add_node(g, 1, 3.0, 0.0);
    int i2 = graph_add_node(g, 2, 3.0, 4.0);
    ASSERT("add node 0 succeeds", i0 == 0);
    ASSERT("add node 1 succeeds", i1 == 1);
    ASSERT("add node 2 succeeds", i2 == 2);
    ASSERT("node count is 3",     graph_node_count(g) == 3);

    // duplicate node rejected
    int dup = graph_add_node(g, 0, 1.0, 1.0);
    ASSERT("duplicate node ID rejected", dup == -1);
    ASSERT("node count still 3",         graph_node_count(g) == 3);

    // find node 
    ASSERT("find node 1 returns index 1",  graph_find_node(g, 1) == 1);
    ASSERT("find non-existent returns -1", graph_find_node(g, 99) == -1);

    // add edges 
    int e0 = graph_add_edge(g, 0, 1, 3.0);
    int e1 = graph_add_edge(g, 1, 2, 4.0);
    int e2 = graph_add_edge(g, 0, 2, 10.0);
    ASSERT("add edge 0->1 succeeds", e0 == 0);
    ASSERT("add edge 1->2 succeeds", e1 == 0);
    ASSERT("add edge 0->2 succeeds", e2 == 0);
    ASSERT("edge count is 3",        graph_edge_count(g) == 3);

    // edge to unknown node rejected
    int bad = graph_add_edge(g, 0, 99, 1.0);
    ASSERT("edge to unknown node rejected", bad == -1);

    // coordinates stored correctly
    ASSERT_NEAR("node 2 x-coord is 3.0", g->nodes[i2].x, 3.0, 1e-9);
    ASSERT_NEAR("node 2 y-coord is 4.0", g->nodes[i2].y, 4.0, 1e-9);

    graph_destroy(g);
    ASSERT("graph_destroy no crash", 1);

    // undirected graph adds reverse edge
    Graph *u = graph_create(0);
    graph_add_node(u, 0, 0.0, 0.0);
    graph_add_node(u, 1, 1.0, 0.0);
    graph_add_edge(u, 0, 1, 5.0);

    int found_reverse = 0;
    EdgeNode *e = u->nodes[1].head;
    while (e) {
        if (e->destination == 0) { found_reverse = 1; break; }
        e = e->next;
    }
    ASSERT("undirected graph adds reverse edge", found_reverse);
    graph_destroy(u);

    test_summary();
}