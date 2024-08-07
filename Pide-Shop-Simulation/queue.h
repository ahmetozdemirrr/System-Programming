#pragma once

/* queue.h */

#ifndef QUEUE_H
#define QUEUE_H

#include "systemHelper.h"

#define MAX_QUE_SIZE 100

struct Queue {
    struct Request **requests;
    int front, rear, size;
    unsigned capacity;
};

int initQueue(struct Queue *);
void destroyQueue(struct Queue *);
int enqueue(struct Queue *, struct Request *);
struct Request *dequeue(struct Queue *);
int isEmpty(struct Queue *);
int isFull(struct Queue *);
int resizeQueue(struct Queue *);

#endif /* QUEUE_H */
