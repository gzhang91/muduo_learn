/*
	Test file:
	gcc test.c blocking_queue.c -o test -lpthread
*/
#include <stdio.h>
#include "blocking_queue.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef int date_type;

void *create_date_elem(int size) {
	int *elem = (int *)malloc(size);
	if (!elem) {
		return NULL;
	}

	return elem;
}

void destroy_data_elem(void *arg) {
	int *elem = (int *)arg;
	free(elem);
}

void *dup_data_elem(void *arg, int size) {
	int *elem = (int *)create_date_elem(size);
	if (!elem) {
		return NULL;
	}

	*elem = *(int *)arg;

	return elem;
}

blocking_queue_t *global_queue_ptr;

void *consumer(void *arg) {
	long sleep_sec = (long)arg;

	while (1) {
		int *data = (int *)Front(global_queue_ptr);
		if (data == NULL) {
			sleep(sleep_sec);
			continue;
		}

		printf("[%ld] consumer: %d\n", sleep_sec, *data);

		PopQueue(global_queue_ptr);
		sleep(sleep_sec);
	}
}

void *producer(void *arg) {
	int count = 0;
	while (1) {
		PushQueue(global_queue_ptr, &count, sizeof(count));
		printf("product: %d\n", count);
		sleep(1);
		count++;
	}
}

int main(void) {
	pthread_t cons1, cons2;
	pthread_t prod1;

	global_queue_ptr = Init(create_date_elem, destroy_data_elem, dup_data_elem);

	long sleep_sec = 1;
	pthread_create(&cons1, NULL, consumer, (void *)sleep_sec);
	sleep_sec = 2;
	pthread_create(&cons2, NULL, consumer, (void *)sleep_sec);
	pthread_create(&prod1, NULL, producer, NULL);

	pthread_join(cons1, NULL);
	pthread_join(cons2, NULL);
	pthread_join(prod1, NULL);

	Destroy(global_queue_ptr);
	
	return 0;
}