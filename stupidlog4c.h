#pragma once

#include <stdio.h>

int stupid_log_init(char *directory, char *prefix);
int stupid_log_close();

FILE *stupid_log_handle();

#define stupid_log(_level, _fmt, ...) do{\
	time_t _t = time(NULL);\
	struct tm _tm; localtime_r(&_t, &_tm);\
	char _tbuf[32];\
	_tbuf[strftime(_tbuf, sizeof(_tbuf), "%F %T", &_tm)] = '\0';\
	fprintf(stupid_log_handle(), "%s %-5s "_fmt"\n", _tbuf, #_level, ##__VA_ARGS__);\
}while(0)

enum STUPID_LOG_LEVELS {
	STUPID_LOG_DEBUG,
	STUPID_LOG_INFO,
	STUPID_LOG_WARN,
	STUPID_LOG_ERROR,
	STUPID_LOG_FATAL,
};

#define STUPID_LOG_MIN_LEVEL STUPID_LOG_DEBUG

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_DEBUG
#define stupid_log_debug(...) stupid_log(DEBUG, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_INFO
#define stupid_log_info(...) stupid_log(INFO, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_WARN
#define stupid_log_warn(...) stupid_log(WARN, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_ERROR
#define stupid_log_error(...) stupid_log(ERROR, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_FATAL
#define stupid_log_fatal(...) stupid_log(FATAL, __VA_ARGS__)
#endif
