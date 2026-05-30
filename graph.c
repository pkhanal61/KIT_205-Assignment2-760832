
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Lifecycle

Graph *graph_create(int is_directed) {
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "graph_create: malloc failed\n");
        return NULL;
    }
    g->num_nodes   = 0;
    g->num_edges   = 0;
    g->is_directed = is_directed;

    // Initialise every node slot to empty
    for (int i = 0; i < MAX_NODES; i++) {
        g->nodes[i].id   = -1;
        g->nodes[i].x    = 0.0;
        g->nodes[i].y    = 0.0;
        g->nodes[i].head = NULL;
    }
    return g;
}

void graph_destroy(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->num_nodes; i++) {
        EdgeNode *cur = g->nodes[i].head;
        while (cur) {
            EdgeNode *tmp = cur->next;
            free(cur);
            cur = tmp;
        }
        g->nodes[i].head = NULL;
    }
    free(g);
}

//Mutation

int graph_add_node(Graph *g, int id, double x, double y) {
    if (!g) return -1;
    if (g->num_nodes >= MAX_NODES) {
        fprintf(stderr, "graph_add_node: MAX_NODES (%d) reached\n", MAX_NODES);
        return -1;
    }
    /* Reject duplicate IDs */
    if (graph_find_node(g, id) >= 0) return -1;

    int idx = g->num_nodes++;
    g->nodes[idx].id   = id;
    g->nodes[idx].x    = x;
    g->nodes[idx].y    = y;
    g->nodes[idx].head = NULL;
    return idx;
}

/* Adds a directed edge src -> dst.
For undirected graphs also adds dst -> src.
Returns 0 on success, -1 on failure.
 */
int graph_add_edge(Graph *g, int src, int dst, double weight) {
    if (!g) return -1;

    int si = graph_find_node(g, src);
    int di = graph_find_node(g, dst);
    if (si < 0 || di < 0) {
        fprintf(stderr, "graph_add_edge: node %d or %d not found\n", src, dst);
        return -1;
    }

    // Allocate and prepend the new edge
    EdgeNode *e = (EdgeNode *)malloc(sizeof(EdgeNode));
    if (!e) return -1;
    e->destination = di;
    e->weight      = weight;
    e->next        = g->nodes[si].head;
    g->nodes[si].head = e;
    g->num_edges++;

    // For undirected graphs, add the reverse edge too 
    if (!g->is_directed) {
        EdgeNode *rev = (EdgeNode *)malloc(sizeof(EdgeNode));
        if (!rev) return -1;
        rev->destination = si;
        rev->weight      = weight;
        rev->next        = g->nodes[di].head;
        g->nodes[di].head = rev;
        //num_edges counts logical edges, not directed copies
    }
    return 0;
}

//Queries

// Linear scan — acceptable for graphs up to a few thousand nodes.
int graph_find_node(const Graph *g, int id) {
    if (!g) return -1;
    for (int i = 0; i < g->num_nodes; i++) {
        if (g->nodes[i].id == id) return i;
    }
    return -1;
}

int graph_node_count(const Graph *g) { return g ? g->num_nodes : 0; }
int graph_edge_count(const Graph *g) { return g ? g->num_edges : 0; }


void graph_print(const Graph *g) {
    if (!g) return;
    printf("Graph (%s, %d nodes, %d edges):\n",
           g->is_directed ? "directed" : "undirected",
           g->num_nodes, g->num_edges);
    for (int i = 0; i < g->num_nodes; i++) {
        printf("  Node %d (%.1f, %.1f) -> ", g->nodes[i].id,
               g->nodes[i].x, g->nodes[i].y);
        EdgeNode *e = g->nodes[i].head;
        while (e) {
            printf("[%d | w=%.2f] ", g->nodes[e->destination].id, e->weight);
            e = e->next;
        }
        printf("\n");
    }
}

/*
Procedural generation — grid graph with density control
Builds a rows x cols grid.  Each potential edge (right/down) is
kept with probability `density`.
Coordinates are the grid (col, row).
Setting is_directed=1 adds both forward and backward directed edges.
*/
Graph *graph_generate_grid(int rows, int cols, double density,
                           int is_directed, unsigned int seed) {
    srand(seed);
    Graph *g = graph_create(is_directed);
    if (!g) return NULL;

    // Add every node
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int id = r * cols + c;
            graph_add_node(g, id, (double)c, (double)r);
        }
    }

    //Guarantee connectivity

    for (int c = 0; c < cols - 1; c++) {
        double w = 1.0 + (double)rand() / RAND_MAX;
        graph_add_edge(g, c, c + 1, w);
        if (is_directed)
            graph_add_edge(g, c + 1, c, w);
    }
    for (int r = 0; r < rows - 1; r++) {
        int src = r * cols + (cols - 1);
        int dst = src + cols;
        double w = 1.0 + (double)rand() / RAND_MAX;
        graph_add_edge(g, src, dst, w);
        if (is_directed)
            graph_add_edge(g, dst, src, w);
    }

    // Add remaining edges 
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int src = r * cols + c;

            // Right edge (skip top row — already guaranteed) 
            if (c + 1 < cols && r > 0) {
                double keep = (double)rand() / RAND_MAX;
                if (keep <= density) {
                    double w = 1.0 + (double)rand() / RAND_MAX;
                    graph_add_edge(g, src, src + 1, w);
                    if (is_directed)
                        graph_add_edge(g, src + 1, src, w);
                }
            }
            // Down edge (skip last col — already guaranteed)
            if (r + 1 < rows && c < cols - 1) {
                double keep = (double)rand() / RAND_MAX;
                if (keep <= density) {
                    double w = 1.0 + (double)rand() / RAND_MAX;
                    graph_add_edge(g, src, src + cols, w);
                    if (is_directed)
                        graph_add_edge(g, src + cols, src, w);
                }
            }
        }
    }
    return g;
}