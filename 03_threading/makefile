# makefile

BINS=blocking_queue_test	\
	count_down_latch_test	\
	thread_local_test

OBJS=blocking_queue.o	\
	count_down_latch.o	\
	thread_local.o

blocking_queue_test: blocking_queue_test.c blocking_queue.c
	gcc -g -o $@ $^ -lpthread
	

count_down_latch_test: count_down_latch_test.c count_down_latch.c
	gcc -g -o $@ $^ -lpthread 

thread_local_test: thread_local_test.c thread_local.c
	gcc -g -o $@ $^ -lpthread 

clean:
	rm -rf $(BINS) $(OBJS)
