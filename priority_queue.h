#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

//Used by both Dijkstra and A*.
//Each entry holds a node index and a priority (cost/f-score).

typedef struct {
    int    node;       // graph node index      
    double priority;   // lower = higher urgency 
} PQEntry;

typedef struct {
    PQEntry *data;
    int      size;
    int      capacity;
} PriorityQueue;

PriorityQueue *pq_create(int capacity);
void           pq_destroy(PriorityQueue *pq);
int            pq_is_empty(const PriorityQueue *pq);
void           pq_push(PriorityQueue *pq, int node, double priority);
PQEntry        pq_pop(PriorityQueue *pq);

#endif 