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

void test_priority_queue(void) {
    printf("\n=== UNIT TESTS: Priority Queue ===\n");

    PriorityQueue *pq = pq_create(8);
    ASSERT("pq_create returns non-NULL", pq != NULL);
    ASSERT("new PQ is empty", pq_is_empty(pq));

    pq_push(pq, 3, 5.0);
    pq_push(pq, 1, 2.0);
    pq_push(pq, 4, 8.0);
    pq_push(pq, 2, 1.0);

    ASSERT("PQ not empty after pushes", !pq_is_empty(pq));

    PQEntry e1 = pq_pop(pq);
    ASSERT("pop returns lowest priority first (node 2)", e1.node == 2);

    PQEntry e2 = pq_pop(pq);
    ASSERT("second pop returns node 1", e2.node == 1);

    PQEntry e3 = pq_pop(pq);
    ASSERT("third pop returns node 3",  e3.node == 3);

    PQEntry e4 = pq_pop(pq);
    ASSERT("fourth pop returns node 4", e4.node == 4);

    ASSERT("PQ empty after all pops", pq_is_empty(pq));

    PQEntry empty = pq_pop(pq);
    ASSERT("pop from empty returns node -1", empty.node == -1);

    pq_destroy(pq);
    ASSERT("pq_destroy no crash", 1);

    test_summary();
}
void test_dijkstra(void) {
    printf("\n=== UNIT TESTS: Dijkstra ===\n");

    Graph *g = graph_create(1);
    graph_add_node(g, 0, 0.0, 0.0);
    graph_add_node(g, 1, 3.0, 0.0);
    graph_add_node(g, 2, 7.0, 0.0);
    graph_add_edge(g, 0, 1, 3.0);
    graph_add_edge(g, 1, 2, 4.0);
    graph_add_edge(g, 0, 2, 10.0);

    DijkstraResult *r = dijkstra_run(g, 0);
    ASSERT("dijkstra_run returns non-NULL", r != NULL);
    ASSERT_NEAR("dist[0] = 0", r->dist[0],  0.0, 1e-9);
    ASSERT_NEAR("dist[1] = 3", r->dist[1],  3.0, 1e-9);
    ASSERT_NEAR("dist[2] = 7", r->dist[2],  7.0, 1e-9);
    ASSERT("nodes_visited == 3", r->nodes_visited == 3);

    //Path reconstruction 
    int path[MAX_NODES];
    int len = dijkstra_get_path(r, 0, 2, path);
    ASSERT("path length 0->2 is 3",    len == 3);
    ASSERT("path[0] = 0 (source)",     len > 0 && path[0] == 0);
    ASSERT("path[1] = 1 (via node 1)", len > 1 && path[1] == 1);
    ASSERT("path[2] = 2 (destination)", len > 2 && path[2] == 2);
    dijkstra_result_free(r);

    // No path (directed graph)
    DijkstraResult *r2 = dijkstra_run(g, 2);
    ASSERT("no path: dist[0] = INF", r2->dist[0] >= INF_DIST);
    int nop = dijkstra_get_path(r2, 2, 0, path);
    ASSERT("get_path returns -1 when no path", nop == -1);
    dijkstra_result_free(r2);

    // Single node graph
    Graph *solo = graph_create(1);
    graph_add_node(solo, 42, 0.0, 0.0);
    DijkstraResult *rs = dijkstra_run(solo, 0);
    ASSERT_NEAR("single node: dist to itself = 0", rs->dist[0], 0.0, 1e-9);
    dijkstra_result_free(rs);
    graph_destroy(solo);

    graph_destroy(g);
    test_summary();
}

void test_astar(void) {
    printf("\n=== UNIT TESTS: A* ===\n");

    // Same graph as Dijkstra test — A* must find same optimal path
    Graph *g = graph_create(1);
    graph_add_node(g, 0, 0.0, 0.0);
    graph_add_node(g, 1, 3.0, 0.0);
    graph_add_node(g, 2, 7.0, 0.0);
    graph_add_edge(g, 0, 1, 3.0);
    graph_add_edge(g, 1, 2, 4.0);
    graph_add_edge(g, 0, 2, 10.0);

    AStarResult *r = astar_run(g, 0, 2, 1.0);
    ASSERT("astar_run returns non-NULL", r != NULL);
    ASSERT("path found",                 r->path_found);
    ASSERT_NEAR("A* cost matches Dijkstra (7)", r->path_cost, 7.0, 1e-9);
    ASSERT("A* nodes_visited <= 3",      r->nodes_visited <= 3);

    int path[MAX_NODES];
    int len = astar_get_path(r, 0, 2, path);
    ASSERT("A* path length is 3",  len == 3);
    ASSERT("A* path[0] = 0",       len > 0 && path[0] == 0);
    ASSERT("A* path[1] = 1",       len > 1 && path[1] == 1);
    ASSERT("A* path[2] = 2",       len > 2 && path[2] == 2);
    astar_result_free(r);

    // No path
    AStarResult *r2 = astar_run(g, 2, 0, 1.0);
    ASSERT("path_found = 0 when no path", !r2->path_found);
    astar_result_free(r2);

    // Weighted A* (w=2.0) still finds a path
    AStarResult *rw = astar_run(g, 0, 2, 2.0);
    ASSERT("weighted A* (w=2) finds a path", rw != NULL && rw->path_found);
    astar_result_free(rw);

    // 4-node graph — verify correct path chosen 
    Graph *g2 = graph_create(1);
    graph_add_node(g2, 0, 0.0, 0.0);
    graph_add_node(g2, 1, 1.0, 0.0);
    graph_add_node(g2, 2, 1.0, 5.0);
    graph_add_node(g2, 3, 2.0, 0.0);
    graph_add_edge(g2, 0, 1, 1.0);
    graph_add_edge(g2, 1, 3, 1.0);
    graph_add_edge(g2, 0, 2, 10.0);
    graph_add_edge(g2, 2, 3, 10.0);

    AStarResult *r3 = astar_run(g2, 0, 3, 1.0);
    ASSERT("A* finds correct path in 4-node graph", r3 != NULL && r3->path_found);
    ASSERT_NEAR("A* cost in 4-node graph = 2", r3->path_cost, 2.0, 1e-9);
    astar_result_free(r3);
    graph_destroy(g2);

    graph_destroy(g);
    test_summary();
}