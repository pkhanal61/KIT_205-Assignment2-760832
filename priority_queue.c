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

// Swaps two entries 
static void swap(PQEntry *a, PQEntry *b) {
    PQEntry tmp = *a;
    *a = *b;
    *b = tmp;
}

// Bubble entry at index i up toward the root
static void sift_up(PriorityQueue *pq, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->data[parent].priority > pq->data[i].priority) {
            swap(&pq->data[parent], &pq->data[i]);
            i = parent;
        } else {
            break;
        }
    }
}

// Push entry at index i down to restore heap property 
static void sift_down(PriorityQueue *pq, int i) {
    while (1) {
        int smallest = i;
        int left  = 2 * i + 1;
        int right = 2 * i + 2;

        if (left  < pq->size && pq->data[left].priority  < pq->data[smallest].priority)
            smallest = left;
        if (right < pq->size && pq->data[right].priority < pq->data[smallest].priority)
            smallest = right;

        if (smallest != i) {
            swap(&pq->data[i], &pq->data[smallest]);
            i = smallest;
        } else {
            break;
        }
    }
}

void pq_push(PriorityQueue *pq, int node, double priority) {
    if (!pq) return;
    // only grows if needed
    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->data = (PQEntry *)realloc(pq->data,
                                       sizeof(PQEntry) * pq->capacity);
        if (!pq->data) {
            fprintf(stderr, "pq_push: realloc failed\n");
            return;
        }
    }
    pq->data[pq->size].node     = node;
    pq->data[pq->size].priority = priority;
    sift_up(pq, pq->size);
    pq->size++;
}

// Removes and returns the entry with the lowest priority
PQEntry pq_pop(PriorityQueue *pq) {
    PQEntry result = {-1, 0.0};
    if (pq_is_empty(pq)) return result;

    result = pq->data[0];
    pq->size--;
    if (pq->size > 0) {
        pq->data[0] = pq->data[pq->size];
        sift_down(pq, 0);
    }
    return result;
}