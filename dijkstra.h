#ifndef DIJKSTRA_H
#define DIJKSTRA_H

/*
It returns shortest distances from a source node to all others,
and the predecessor array for path reconstruction.
Also it records nodes visited so we can compare with A*.
*/

#include "graph.h"

#define INF_DIST 1e18   /* no path found */

typedef struct {
    double *dist;           // dist[i]  = shortest distance to node i 
    int    *prev;           // prev[i]  = predecessor index (-1 = none)
    int     nodes_visited;  // how many nodes were popped from the PQ   
    int     num_nodes;      // length of dist[] and prev[]              
} DijkstraResult;

DijkstraResult *dijkstra_run(const Graph *g, int src_index);
void            dijkstra_result_free(DijkstraResult *r);
int dijkstra_get_path(const DijkstraResult *r, int src_index,
                      int dst_index, int *path);
void dijkstra_print_path(const Graph *g, const DijkstraResult *r,
                         int src_index, int dst_index);
#endif