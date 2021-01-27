#include "thread.h"
#include <stdio.h>

void *thr_fn(void *arg) {
	printf("hello world\n");
}

int main(int argc, char **argv) {
	thread_t *thr = create_thread(0, thr_fn, NULL);
	
	start_thread(thr);
	wait_thread(thr);

	destroy_thread(thr);

	return 0;
}