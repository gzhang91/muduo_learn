#ifndef _COUNT_DOWN_LATCH_H_
#define _COUNT_DOWN_LATCH_H_

#include <pthread.h>

struct count_down_latch_s {
	pthread_mutex_t *lock;
	pthread_cond_t *cond;
	int count_down_num;
};

typedef struct count_down_latch_s count_down_latch_t;

count_down_latch_t *InitCountDownLatch(int num_of_count);
void DestroyCountDownLatch(count_down_latch_t *cdt);

void Wait(count_down_latch_t *cdt);
int GetCountDown(count_down_latch_t *cdt);
void CountDown(count_down_latch_t *cdt);

#endif // COUNT_DOWN_LATCH_H_