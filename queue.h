#ifndef QUEUE_H
#define QUEUE_H
 
#include <stdlib.h>

typedef struct {
  unsigned int head;
  unsigned int tail;
  unsigned int size;
  unsigned int items;
  int *data;
} queue_t;

queue_t *create_queue(int size) {
  queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
  queue->head = 0;
  queue->tail = 0;
  queue->size = size;
  queue->data = (int *)malloc(size * sizeof(int));
  queue->items = 0;
  int i;//loop iterator
  for(i = 0; i < size; i++)
    queue->data[i] = -1;//set all elements to -1. no pids will be -1
  return queue;
}

void enqueue(queue_t *queue, int pid) {
  queue->data[queue->tail] = pid;//add pid to queue
  queue->tail = (queue->tail + 1) % queue->size;
  queue->items += 1; 
  return;
}

int dequeue(queue_t *queue) {
  int pid = queue->data[queue->head];
  queue->head = (queue->head + 1) % queue->size;
  queue->items -= 1; 
  return pid;
}
#endif 
