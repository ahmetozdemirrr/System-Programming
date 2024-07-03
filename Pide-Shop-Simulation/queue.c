#include "queue.h"
#include "systemHelper.h"

int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

int isFull(struct Queue *queue)
{
    return (queue->size == queue->capacity);
}

int initQueue(struct Queue *queue)
{
    if ((queue->requests = (struct Request **)malloc(MAX_QUE_SIZE * sizeof(struct Request *))) == NULL)
    {
        perror("memory allocation error!");
        return -1;
    }
    queue->front = queue->rear = queue->size = 0;
    queue->capacity = MAX_QUE_SIZE;
    return 0;
}

void destroyQueue(struct Queue *queue)
{
    for (int i = 0; i < queue->size; ++i)
    {
        free(queue->requests[i]);
    }
    free(queue->requests);
}

int enqueue(struct Queue *queue, struct Request *request)
{
    if ((queue->size == queue->capacity) && (resizeQueue(queue) == -1))
    {
        return -1;
    }
    queue->requests[queue->rear] = request;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
    return 0;
}

struct Request *dequeue(struct Queue *queue)
{
    if (queue->size == 0)
    {
        return NULL;
    }
    struct Request *request = queue->requests[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return request;
}

int resizeQueue(struct Queue *queue)
{
    int newCapacity = (queue->capacity == 0) ? 1 : (2 * queue->capacity);
    struct Request **newRequests;

    if ((newRequests = malloc(newCapacity * sizeof(struct Request *))) == NULL)
    {
        perror("memory allocation error!");
        return -1;
    }

    for (int i = 0; i < queue->size; ++i)
    {
        newRequests[i] = queue->requests[(queue->front + i) % queue->capacity];
    }
    free(queue->requests);
    queue->requests = newRequests;
    queue->capacity = newCapacity;
    queue->front = 0;
    queue->rear = queue->size;
    return 0;
}
