# makefile

BINS=log_test

OBJS=logger.o			\
	log_buffer.o		\
	log_formatter.o		\
	../03_threading/thread.o

log_test: log_test.c logger.c log_buffer.c log_formatter.c ../03_threading/thread.c
	gcc -g -o $@ $^ -lpthread

clean:
	rm -rf $(BINS) $(OBJS)
