#include "blocking_queue.h"
#include <stdlib.h>

blocking_queue_t *Init(create_fn create, destroy_fn destroy, dup_fn dup) {
	blocking_queue_t *queue = (blocking_queue_t*)malloc(sizeof(*queue));
	if (!queue) {
		return NULL;
	}

	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);

	queue->header = queue->tailer = NULL;
	queue->size = 0;

	queue->create_handler = create;
	queue->destroy_handler = destroy;
	queue->dup_handler = dup;
	return queue;
}

void Destroy(blocking_queue_t *queue) {
	if (!queue) {
		return;
	}

	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->cond);

	while (!Empty(queue)) {
		PopQueue(queue);
	}
}

void Lock(blocking_queue_t *queue) {
	if (!queue) {
		return;
	}

	pthread_mutex_lock(&queue->mutex);
}

void UnLock(blocking_queue_t *queue) {
	if (!queue) {
		return;
	}

	pthread_mutex_unlock(&queue->mutex);
}

int Wait(blocking_queue_t *queue) {
	return pthread_cond_wait(&queue->cond, &queue->mutex);
}

void Notify(blocking_queue_t *queue) {
	pthread_cond_signal(&queue->cond);
}

void NotifyAll(blocking_queue_t *queue) {
	pthread_cond_broadcast(&queue->cond);
}

void PushQueue(blocking_queue_t *queue, void *data, int size) {
	Lock(queue);

	if (!queue) {
		goto exit;
	}

	node_t *new_node = (node_t*)malloc(sizeof(*new_node));
	if (!new_node) {
		goto exit;
	}

	new_node->data = queue->dup_handler(data, size);

	new_node->prev = queue->tailer;
	if (new_node->prev) {
		new_node->prev->next = new_node;
	}

	new_node->next = NULL;
	queue->tailer = new_node;

	if (!queue->header) {
		queue->header = new_node;
	}

	queue->size ++;

exit:
	UnLock(queue);
	NotifyAll(queue);
}

void *Front(blocking_queue_t *queue) {
	Lock(queue);

	void *data = NULL;
	if (!queue || !queue->header) {
		goto exit;
	}
	
	data = queue->header->data;
exit:
	UnLock(queue);
	
	return data;
}

void PopQueue(blocking_queue_t *queue) {
	Lock(queue);

	if (!queue) {
		goto exit;
	}

	while (!queue->header) {
		Wait(queue);
	}

	node_t *tmp = queue->header;
	if (tmp->next) {
		tmp->next->prev = NULL;
	}

	queue->header = tmp->next;

	queue->destroy_handler(tmp->data);
	free(tmp);
	tmp = NULL;
	queue->size --;

exit:
	UnLock(queue);
}

int GetQueueSize(blocking_queue_t *queue) {
	Lock(queue);
	
	int size = 0;

	if (!queue) {
		goto exit;
	}
	size = queue->size;

exit:
	UnLock(queue);
	return size;
}

int Empty(blocking_queue_t *queue) {
	Lock(queue);

	int empty = 0;
	if (!queue) {
		goto exit;
	}
	
	empty = (queue->size == 0 ? 0 : 1);

exit:
	UnLock(queue);

	return empty;
}