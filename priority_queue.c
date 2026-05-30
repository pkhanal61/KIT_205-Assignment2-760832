#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

PriorityQueue *pq_create(int capacity) {
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    pq->data = (PQEntry *)malloc(sizeof(PQEntry) * capacity);
    if (!pq->data) { free(pq); return NULL; }
    pq->size     = 0;
    pq->capacity = capacity;
    return pq;
}

void pq_destroy(PriorityQueue *pq) {
    if (!pq) return;
    free(pq->data);
    free(pq);
}

int pq_is_empty(const PriorityQueue *pq) {
    return !pq || pq->size == 0;
}

