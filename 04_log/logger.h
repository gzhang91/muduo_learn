#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdarg.h>
#include <stdlib.h>
#include "log_buffer.h"
#include "log_formatter.h"

/*
	非线程安全，只能用在多进程中
*/

typedef void (*output_handler)(int fd, const char *msg, int len);

struct logger_s {
	int log_level;
	log_buffer_t buffer;
	log_formatter_t formatter;
	char file_name[1024];
	int log_fd;

	output_handler output; 
};

typedef struct logger_s logger_t;

extern logger_t g_log;

void LOG_FATAL(const char *fmt, ...);
void LOG_ERROR(const char *fmt, ...);
void LOG_WARN(const char *fmt, ...);
void LOG_INFO(const char *fmt, ...);
void LOG_DEBUG(const char *fmt, ...); 

// LOG FUNCTION
void InitLog(const char *filename, int log_level, output_handler o);
void SetLogLevel(int log_level);

void LogCore(logger_t *log, const char *fmt, va_list args);

void DefaultOutputHandler(int fd, const char *msg, int len);


#endif // _LOGGER_H_