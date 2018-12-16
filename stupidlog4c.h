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
