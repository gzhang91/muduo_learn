#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

#include <stdio.h>
#include <stdarg.h>
#include "log_formatter.h"

int Format(log_formatter_t *fmter, const char *fmt, va_list args) {
	int formatted = 0;

	char *buf = fmter->buf;

	// 格式化日志等级
	switch (fmter->log_level) {
		case FATAL:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[FATAL  ] ");
			break;

		case ERROR:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[ERROR  ] ");
			break;

		case WARN:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[WARN   ] ");
			break;

		case INFO:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[INFO   ] ");
			break;

		case DEBUG:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[DEBUG  ] ");
			break;

		default:
			formatted += snprintf(buf + formatted, fmter->max_len - formatted, "[UNKNOWN] ");
			break;
	}

	// 格式化文件
	//formatted += snprintf(buf + formatted, fmter->max_len - formatted, "%s ", __FILE__);

	// 格式化行
	//formatted += snprintf(buf + formatted, fmter->max_len - formatted, "%d ", __LINE__);

	// 格式化func
	// formatted += snprintf(buf + formatted, fmter->max_len - formatted, "%s ", __func__); 

	// 格式化pid
	if (!fmter->pid) {
		fmter->pid = (pid_t)syscall(__NR_gettid);
	}

	formatted += snprintf(buf + formatted, fmter->max_len - formatted, "%d ", (int)fmter->pid);

	// 格式化用户的内容
	formatted += vsnprintf(buf + formatted, fmter->max_len - formatted, fmt, args);

	return formatted;
}