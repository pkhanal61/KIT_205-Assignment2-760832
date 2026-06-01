#ifndef ASTAR_H
#define ASTAR_H

#include "graph.h"

#define ASTAR_INF 1e18

/*
 Stores the result of running A* from source to destination.
g_cost[] holds best known costs, prev[] holds predecessors.
nodes_visited tracks efficiency vs Dijkstra.
 */
typedef struct {
    double *g_cost;        // best known cost to each node   
    int    *prev;          // predecessor index (-1 = none)    
    int     nodes_visited; // nodes popped from open set       
    int     num_nodes;
    int     path_found;    // 1 if destination was reached     
    double  path_cost;     // final cost to destination        
} AStarResult;
AStarResult *astar_run(const Graph *g, int src_index, int dst_index,
                        double weight);
void          astar_result_free(AStarResult *r);

#endif 