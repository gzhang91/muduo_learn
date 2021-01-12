#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include "pthread.h"

typedef void *(*create_fn)(int size);
typedef void (*destroy_fn)(void *arg);
typedef void *(*dup_fn)(void *arg, int size);

struct node_s {
	void *data;
	struct node_s *prev;
	struct node_s *next;
};

typedef struct node_s node_t;

struct blocking_queue_s {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	
	node_t *header, *tailer;
	int size;

	// func pointer
	create_fn create_handler;
	destroy_fn destroy_handler;
	dup_fn dup_handler;
};

typedef struct blocking_queue_s blocking_queue_t;

blocking_queue_t *Init(create_fn create, destroy_fn destroy, dup_fn dup);
void Destroy(blocking_queue_t *queue);
void PushQueue(blocking_queue_t *queue, void *data, int size);
void *Front(blocking_queue_t *queue);
void PopQueue(blocking_queue_t *queue);

void Lock(blocking_queue_t *queue);
void UnLock(blocking_queue_t *queue);
int Wait(blocking_queue_t *queue);
void Notify(blocking_queue_t *queue);
void NotifyAll(blocking_queue_t *queue);

int GetQueueSize(blocking_queue_t *queue);
int Empty(blocking_queue_t *queue);

#endif // _BLOCKING_QUEUE_H_