#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "logger.h"

int log_init_flag = 0;
int g_log_level = INFO;
logger_t g_log;

void LogCore(logger_t *log, const char *fmt, va_list args) {
	do {
		if (!(log)) {		
			break;			
		}			
			
		(log)->formatter.buf = (log)->buffer.data;	
		(log)->formatter.log_level = (log)->log_level;	
		(log)->formatter.max_len = MAX_BUF_SIZE;	
		int fort_len = Format(&(log)->formatter, (fmt), args);	
		(log)->output((log)->log_fd, (log)->buffer.data, fort_len);
	} while(0);
}

void InitLog(const char *filename, int log_level, output_handler o) {
	int use_stdout = 0;
	if (!filename) {
		use_stdout = 1;
	}

	if (log_init_flag) {
		if (strcmp(filename, g_log.file_name) == 0) {
			return;
		}

		// 将没有写完的日志写入到文件中
	}
	
	memset(&g_log.buffer, 0x0, sizeof(g_log.buffer));
	memset(&g_log.formatter, 0x0, sizeof(g_log.formatter));
	memset(g_log.file_name, 0x0, sizeof(g_log.file_name));

	if (g_log.log_fd > 2) {
		close(g_log.log_fd);
	}

	g_log.log_fd = open(filename, O_CREAT | O_RDWR | O_APPEND, 0660);
	if (g_log.log_fd < 0) {
		printf("open file[%s] failed, use stdout.\n", filename);
		g_log.log_fd = STDOUT_FILENO;
	} else {
		memcpy(g_log.file_name, filename, sizeof(g_log.file_name) - 1);
	}

	g_log.log_level = log_level;
	g_log.output = &DefaultOutputHandler;
	if (o) {
		g_log.output = o;
	}

	log_init_flag = 1;
}

void SetLogLevel(int log_level) {
	g_log_level = log_level;
}

void DefaultOutputHandler(int fd, const char *msg, int len) {
	Flush1(fd, msg, len);
}

void LOG_FATAL(const char *fmt, ...) {
	if (!log_init_flag) {	
		InitLog(NULL, g_log_level, NULL);	
	}

	if (g_log.log_level >= FATAL) { 
		va_list args;	
		va_start(args, fmt);
		g_log.log_level = FATAL;
		LogCore(&g_log, fmt, args);
		g_log.log_level = g_log_level;
		va_end(args);	
		abort(); 
	}
}

void LOG_ERROR(const char *fmt, ...) {
	if (!log_init_flag) {	
		InitLog(NULL, g_log_level, NULL);	
	}

	if (g_log.log_level >= ERROR) { 
		va_list args;	
		va_start(args, fmt);	
		g_log.log_level = ERROR;
		LogCore(&g_log, fmt, args);
		g_log.log_level = g_log_level;
		va_end(args);		
	} 
}

void LOG_WARN(const char *fmt, ...) {
	if (!log_init_flag) {	
		InitLog(NULL, g_log_level, NULL);	
	}

	if (g_log.log_level >= WARN) {
		va_list args;
		va_start(args, fmt);
		g_log.log_level = WARN;
		LogCore(&g_log, fmt, args);
		g_log.log_level = g_log_level;
		va_end(args);
	} 
}

void LOG_INFO(const char *fmt, ...) {
	if (!log_init_flag) {	
		InitLog(NULL, g_log_level, NULL);	
	}

	if (g_log.log_level >= INFO) { 
		va_list args;
		va_start(args, fmt);
		g_log.log_level = INFO;
		LogCore(&g_log, fmt, args);
		g_log.log_level = g_log_level;
		va_end(args);
	}
}

void LOG_DEBUG(const char *fmt, ...) {
	if (!log_init_flag) {	
		InitLog(NULL, g_log_level, NULL);	
	}

	if (g_log.log_level >= DEBUG) { 
		va_list args;	
		va_start(args, fmt);
		g_log.log_level = DEBUG;
		LogCore(&g_log, fmt, args);
		g_log.log_level = g_log_level;
		va_end(args);
	}
}
