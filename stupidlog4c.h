#pragma once

#include <stdio.h>

int stupid_log_init(char *directory, char *prefix);
int stupid_log_close();

FILE *stupid_log_handle();

#define stupid_log(_level, _fmt, ...) do{\
	fprintf(stupid_log_handle(), "[%-5s] "_fmt"\n", #_level, ##__VA_ARGS__);\
}while(0)
