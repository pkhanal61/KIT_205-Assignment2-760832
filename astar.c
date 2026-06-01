#include "astar.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Euclidean distance between two graph nodes 
static double euclidean(const Graph *g, int a, int b) {
    double dx = g->nodes[a].x - g->nodes[b].x;
    double dy = g->nodes[a].y - g->nodes[b].y;
    return sqrt(dx * dx + dy * dy);
}

AStarResult *astar_run(const Graph *g, int src_index, int dst_index,
                        double weight) {
    if (!g || src_index < 0 || src_index >= g->num_nodes) return NULL;
    if (dst_index < 0 || dst_index >= g->num_nodes)       return NULL;

    int n = g->num_nodes;

    AStarResult *r = (AStarResult *)malloc(sizeof(AStarResult));
    if (!r) return NULL;

    r->g_cost        = (double *)malloc(sizeof(double) * n);
    r->prev          = (int *)   malloc(sizeof(int)    * n);
    r->nodes_visited = 0;
    r->num_nodes     = n;
    r->path_found    = 0;
    r->path_cost     = ASTAR_INF;

    if (!r->g_cost || !r->prev) { astar_result_free(r); return NULL; }

    for (int i = 0; i < n; i++) {
        r->g_cost[i] = ASTAR_INF;
        r->prev[i]   = -1;
    }
    r->g_cost[src_index] = 0.0;

    int *closed = (int *)calloc(n, sizeof(int));
    if (!closed) { astar_result_free(r); return NULL; }

    PriorityQueue *open = pq_create(n * 2 + 1);
    if (!open) { free(closed); astar_result_free(r); return NULL; }

    double h0 = euclidean(g, src_index, dst_index);
    pq_push(open, src_index, weight * h0);

    while (!pq_is_empty(open)) {
        PQEntry entry = pq_pop(open);
        int u = entry.node;

        if (closed[u]) continue;
        closed[u] = 1;
        r->nodes_visited++;

        // Early exit once destination is reached 
        if (u == dst_index) {
            r->path_found = 1;
            r->path_cost  = r->g_cost[dst_index];
            break;
        }

        EdgeNode *edge = g->nodes[u].head;
        while (edge) {
            int    v  = edge->destination;
            double tg = r->g_cost[u] + edge->weight;
            if (tg < r->g_cost[v]) {
                r->g_cost[v] = tg;
                r->prev[v]   = u;
                double h = euclidean(g, v, dst_index);
                pq_push(open, v, tg + weight * h);
            }
            edge = edge->next;
        }
    }

    pq_destroy(open);
    free(closed);
    return r;
}

void astar_result_free(AStarResult *r) {
    if (!r) return;
    free(r->g_cost);
    free(r->prev);
    free(r);
}