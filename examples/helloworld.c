#include <stdlib.h>
#include <stdio.h>

#include "../stupidlog4c.h"

int main() {
	if (stupid_log_init("/tmp", "HelloWorldService.log", STUPID_LOG_HOURLY) < 0) {
		perror("stupid_log_init");
		exit(1);
	}
	atexit(stupid_log_close);

	stupid_log_info("Hello, World!");
}
