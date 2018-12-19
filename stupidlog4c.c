#include "stupidlog4c.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static char filenameprefix[256] = {0};
static enum STUPID_LOG_ROLLOVER rollover_granularity = STUPID_LOG_HOURLY;
static _Thread_local struct tm current_log_tm = {0};
static _Thread_local FILE *logfile = NULL;

static bool should_rollover(struct tm *old, struct tm *new) {
	if (rollover_granularity >= STUPID_LOG_HOURLY && old->tm_hour != new->tm_hour) {
		return true;
	}
	if (rollover_granularity >= STUPID_LOG_DAILY && old->tm_mday != new->tm_mday) {
		return true;
	}
	if (rollover_granularity >= STUPID_LOG_MONTHLY && old->tm_mon != new->tm_mon) {
		return true;
	}
	if (rollover_granularity >= STUPID_LOG_YEARLY && old->tm_year != new->tm_year) {
		return true;
	}
	return false;
}

static FILE *stupid_log_make_handle() {
	time_t t;
	struct tm tm;
	char tbuf[16];
	char pathbuf[256+16];
	FILE *f;

	t = time(NULL);
	localtime_r(&t, &tm);
	tbuf[strftime(tbuf, sizeof(tbuf), "%F-%H", &tm)] = '\0';

	snprintf(pathbuf, sizeof(pathbuf), "%s.%s", filenameprefix, tbuf);

	f = fopen(pathbuf, "a");
	if (f == NULL) {
		return stderr;
	}
	setlinebuf(f);
	current_log_tm = tm;
	return f;
}

static int safe_fclose(FILE *f) {
	if (f != NULL && f != stderr && f != stdout) {
		return fclose(f);
	}
	return 0;
}

static FILE *stupid_log_handle(struct tm *new_tm) {
	if (should_rollover(&current_log_tm, new_tm)) {
		// If it's time to rollover, close the old handle and let the next block
		// open a new handle.
		safe_fclose(logfile);
		logfile = NULL;
	}
	if (logfile == NULL) {
		logfile = stupid_log_make_handle();
	}
	return logfile;
}

int stupid_log_init(char *directory, char *prefix, enum STUPID_LOG_ROLLOVER rollover) {
	char pathbuf[256];
	FILE *tmp;

	rollover_granularity = rollover;

	// Check if the directory is writeable
	snprintf(pathbuf, sizeof(pathbuf), "%s/%s", directory, ".stupidlog4c_tmp");
	tmp = fopen(pathbuf, "w");
	if (tmp == NULL) {
		return -1;
	}
	if (fputc('\n', tmp) == EOF) {
		return -1;
	}
	fclose(tmp);
	remove(pathbuf);

	// Save file prefix
	snprintf(filenameprefix, sizeof(filenameprefix), "%s/%s", directory, prefix);
	return 0;
}

int stupid_log_close() {
	int rc = safe_fclose(logfile);
	logfile = NULL;
	return rc;
}

void stupid_log(const char *level, const char *format, ...) {
	time_t t;
	struct tm tm;
	char tbuf[32];
	FILE *out;
	va_list args;

	t = time(NULL);
	localtime_r(&t, &tm);
	tbuf[strftime(tbuf, sizeof(tbuf), "%F %T", &tm)] = '\0';

	out = stupid_log_handle(&tm);
	fprintf(out, "%s [%-5s] ", tbuf, level);
	va_start(args, format);
	vfprintf(out, format, args);
	va_end(args);
	fputc('\n', out);
}
