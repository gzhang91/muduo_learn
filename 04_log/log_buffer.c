#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "log_buffer.h"

void Flush(log_buffer_t *buf, int fd) {
	if (!buf) {
		return;
	}

	int retlen = 0;

	time_t now = time(NULL);
	if (now > buf->next_flush + buf->time_span) {
		return;
	}

	retlen = write(fd, buf->data + buf->curr_pos, buf->data_len - buf->curr_pos);
	if (retlen == buf->data_len - buf->curr_pos) {
		buf->curr_pos = 0;
		return;
	}

	buf->curr_pos += retlen;
}

void Flush1(int fd, const char *msg, int len) {
	int retlen = 0, retcur = 0;

	while (retlen < len) {
		retcur = write(fd, msg + retlen, len - retlen);
		if (retcur < 0) {
			printf("error accurred, errno: %d\n", errno);
			break;
		}

		retlen += retcur;
	}

	fsync(fd);
}