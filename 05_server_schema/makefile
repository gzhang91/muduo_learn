# makefile

BINS=iterator_svr \
	concurrent_svr 

iterator_svr: 01iterator.c 00common.c
	gcc -g -o $@ $^

concurrent_svr: 02concurrent.c 00common.c ../03_threading/thread.c
	gcc -g -o $@ $^ -lpthread
	
prefork_svr: 03prefork.c 00common.c 00channel.c
	gcc -g -o $@ $^
	
clean:
	rm -rf $(BINS) 00common.o
