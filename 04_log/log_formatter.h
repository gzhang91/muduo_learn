#ifndef _LOG_FORMATTER_H_
#define _LOG_FORMATTER_H_

#include <sys/types.h>

enum LOG_LEVEL {
	FATAL,
	ERROR,
	WARN,
	INFO,
	DEBUG,
};


struct log_formatter_s {
	int log_level;
	char *buf;
	int max_len;

	pid_t pid; // ½ø³Ìpid
};

typedef struct log_formatter_s log_formatter_t;

int Format(log_formatter_t *fmter, const char *fmt, va_list args);

#endif // _LOG_FORMATTER_H_