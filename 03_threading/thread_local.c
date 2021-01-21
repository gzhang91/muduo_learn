#include "thread_local.h"
#include <stdlib.h>

thread_local_t *InitThreadLocal(malloc_fn mfn, free_fn ffn) {
	if (!mfn || !ffn) {
		return NULL;
	}

	thread_local_t *tl = (thread_local_t *)malloc(sizeof(*tl));
	if (!tl) {
		return NULL;
	}

	int ret = pthread_key_create(&tl->key, NULL);
	if (ret < 0) {
		goto exit;
	}

	s_malloc_handler = mfn;
	s_free_handler = ffn;

	return tl;

exit:
	pthread_key_delete(tl->key);
	free(tl);
	return NULL;
}

void DestroyThreadLocal(thread_local_t *tl) {
	if (!tl) {
		return;
	}

	void *data = Value(tl);
	s_free_handler(data);

	pthread_key_delete(tl->key);
	free(tl);
}

void *Value(thread_local_t *tl) {
	if (!tl) {
		return NULL;
	}

	void *data = pthread_getspecific(tl->key);
	if (data) {
		return data;
	}
	data = s_malloc_handler(NULL);
	pthread_setspecific(tl->key, data);

	return data;
}