#ifndef GRAPH_H
#define GRAPH_H


#define MAX_NODES 2000

typedef struct EdgeNode {
    int destination;        // index of the destination node 
    double weight;          
    struct EdgeNode *next;
} EdgeNode;

/* One node in the graph */
typedef struct {
    int    id;             
    double x, y;            //coordinates for A* heuristic  
    EdgeNode *head;         // head of the adjacency list
} Node;


typedef struct {
    Node   nodes[MAX_NODES];
    int    num_nodes;
    int    num_edges;
    int    is_directed;     // 1 = directed, 0 = undirected   
} Graph;

// lifecycle
Graph *graph_create(int is_directed);
void   graph_destroy(Graph *g);

// mutation
int  graph_add_node(Graph *g, int id, double x, double y);
int  graph_add_edge(Graph *g, int src, int dst, double weight);

// queries
int  graph_find_node(const Graph *g, int id);
int  graph_node_count(const Graph *g);
int  graph_edge_count(const Graph *g);

//display
void graph_print(const Graph *g);

//procedural generation
Graph *graph_generate_grid(int rows, int cols, double density,
                           int is_directed, unsigned int seed);

#endif