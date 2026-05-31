#include "dijkstra.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>

DijkstraResult *dijkstra_run(const Graph *g, int src_index) {
    if (!g || src_index < 0 || src_index >= g->num_nodes) return NULL;

    int n = g->num_nodes;

    DijkstraResult *r = (DijkstraResult *)malloc(sizeof(DijkstraResult));
    if (!r) return NULL;

    r->dist          = (double *)malloc(sizeof(double) * n);
    r->prev          = (int *)   malloc(sizeof(int)    * n);
    r->nodes_visited = 0;
    r->num_nodes     = n;

    if (!r->dist || !r->prev) {
        dijkstra_result_free(r);
        return NULL;
    }

    // Initialise distances to infinity and predecessors to -1
    for (int i = 0; i < n; i++) {
        r->dist[i] = INF_DIST;
        r->prev[i] = -1;
    }
    r->dist[src_index] = 0.0;

    // visited[i] = 1 once a node's shortest path is finalised 
    int *visited = (int *)calloc(n, sizeof(int));
    if (!visited) { dijkstra_result_free(r); return NULL; }

    PriorityQueue *pq = pq_create(n * 2 + 1);
    if (!pq) { free(visited); dijkstra_result_free(r); return NULL; }

    pq_push(pq, src_index, 0.0);

    while (!pq_is_empty(pq)) {
        PQEntry entry = pq_pop(pq);
        int u = entry.node;

        // Skip stale entries
        if (visited[u]) continue;
        visited[u] = 1;
        r->nodes_visited++;

        //Relax outgoing edges 
        EdgeNode *edge = g->nodes[u].head;
        while (edge) {
            int    v     = edge->destination;
            double new_d = r->dist[u] + edge->weight;
            if (new_d < r->dist[v]) {
                r->dist[v] = new_d;
                r->prev[v] = u;
                pq_push(pq, v, new_d);
            }
            edge = edge->next;
        }
    }

    pq_destroy(pq);
    free(visited);
    return r;
}
void dijkstra_result_free(DijkstraResult *r) {
    if (!r) return;
    free(r->dist);
    free(r->prev);
    free(r);
}
