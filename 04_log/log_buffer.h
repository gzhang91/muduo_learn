#ifndef _LOG_BUFFER_H_
#define _LOG_BUFFER_H_

#include <time.h>

#define MAX_BUF_SIZE 2048

struct log_buffer_s {
	// a free page size
	char data[MAX_BUF_SIZE];
	int data_len;
	int curr_pos;

	time_t next_flush;
	int time_span;
};

typedef struct log_buffer_s log_buffer_t;

void Flush(log_buffer_t *buf, int fd);
void Flush1(int fd, const char *msg, int len);

#endif // _LOG_BUFFER_H_