#include "count_down_latch.h"
#include <stdlib.h>

count_down_latch_t *InitCountDownLatch(int num_of_count) {
	if (num_of_count <= 0) {
		return NULL;
	}

	count_down_latch_t *cdt = (count_down_latch_t *)malloc(sizeof(*cdt));
	if (!cdt) {
		return NULL;
	}
	cdt->cond = NULL;
	cdt->lock = NULL;
	cdt->count_down_num = 0;
	
	cdt->lock = (pthread_mutex_t *)malloc(sizeof(*cdt->lock));
	if (!cdt->lock) {
		goto exit;
	}

	// 暂且先用默认属性吧
	pthread_mutex_init(cdt->lock, NULL);

	cdt->cond = (pthread_cond_t *)malloc(sizeof(*cdt->cond));
	if (!cdt->cond) {
		goto exit;
	}

	pthread_cond_init(cdt->cond, NULL);
	cdt->count_down_num = num_of_count;
	
	return cdt;

exit:
	if (cdt->lock) {
		pthread_mutex_destroy(cdt->lock);
		free(cdt->lock);
	}
	
	if (cdt->cond) {
		pthread_cond_destroy(cdt->cond);
		free(cdt->cond);
	}

	free(cdt);
	cdt = NULL;
}

void DestroyCountDownLatch(count_down_latch_t *cdt) {
	if (!cdt) {
		return;
	}

	if (cdt->cond) {
		pthread_cond_broadcast(cdt->cond);
		pthread_cond_destroy(cdt->cond);
		cdt->cond = NULL;
	}

	if (cdt->lock) {
		pthread_mutex_destroy(cdt->lock);
		free(cdt->lock);
		cdt->lock = NULL;
	}

	free(cdt);
	cdt = NULL;
}

void Wait(count_down_latch_t *cdt) {
	if (!cdt) {
		return;
	}

	pthread_mutex_lock(cdt->lock);

	while (cdt->count_down_num > 0) {
		pthread_cond_wait(cdt->cond, cdt->lock);
	}

	pthread_mutex_unlock(cdt->lock);
}

int GetCountDown(count_down_latch_t *cdt) {
	if (!cdt) {
		return 0;
	}

	int cnt = 0;
	pthread_mutex_lock(cdt->lock);
	cnt = cdt->count_down_num;
	pthread_mutex_unlock(cdt->lock);
	
	return cnt;
}

void CountDown(count_down_latch_t *cdt) {
	if (!cdt) {
		return;
	}

	int cnt = GetCountDown(cdt);
	if (cnt <= 0) {
		return;
	}

	pthread_mutex_lock(cdt->lock);
	cdt->count_down_num--;
	if (cdt->count_down_num <= 0) {
		pthread_cond_broadcast(cdt->cond);
	}

	pthread_mutex_unlock(cdt->lock);
}