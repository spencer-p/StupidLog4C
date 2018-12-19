/*
 * A log forwarder that reads standard input and aggregates into log files.
 *
 * Inspired by the original BSD utility (and subsequently Linux) by the same
 * name which writes to the syslog.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../stupidlog4c.h"

int main(int argc, char **argv) {
	char linebuf[512];

	if (argc < 3) {
		printf("Not enough arguments\n");
		exit(1);
	}

	if (stupid_log_init(argv[1], argv[2], STUPID_LOG_HOURLY) < 0) {
		perror("stupid_log_init");
		exit(1);
	}
	atexit(stupid_log_close);
	
	while (fgets(linebuf, sizeof(linebuf), stdin) != NULL) {
		/* Remove the newline */
		linebuf[strlen(linebuf)-1] = '\0';
		stupid_log_info(linebuf);
	}
}
