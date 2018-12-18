/*
 * Copyright (c) 2018 Spencer Peterson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stupidlog4c.h"

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define PATH_BUFSIZE 256

static char filenameprefix[PATH_BUFSIZE] = {0};
static enum STUPID_LOG_ROLLOVER rollover_granularity = STUPID_LOG_HOURLY;
static _Thread_local struct tm current_log_tm = {0};
static _Thread_local FILE *logfile = NULL;

static FILE *stupid_log_handle(struct tm *new_tm);
static FILE *stupid_log_make_handle();
static bool should_rollover(struct tm *old, struct tm *new);
static int safe_fclose(FILE *f);
static int strjoin(char *dest, size_t n, char *left, char mid, char *right);

static FILE *stupid_log_handle(struct tm *new_tm) {
	if (should_rollover(&current_log_tm, new_tm)) {
		/* If rolling over, close the stream and let the next block open it */
		safe_fclose(logfile);
		logfile = NULL;
	}
	if (logfile == NULL) {
		logfile = stupid_log_make_handle();
	}
	return logfile;
}

static FILE *stupid_log_make_handle() {
	time_t t;
	struct tm *tm;
	char tbuf[16];
	char pathbuf[PATH_BUFSIZE+16];
	FILE *f;

	t = time(NULL);
	tm = localtime(&t);
	tbuf[strftime(tbuf, sizeof(tbuf), "%F-%H", tm)] = '\0';

	strjoin(pathbuf, sizeof(pathbuf), filenameprefix, '.', tbuf);

	f = fopen(pathbuf, "a");
	if (f == NULL) {
		return stderr;
	}

	/* Flush log lines immediately by setting line buffering */
	setvbuf(f, NULL, _IOLBF, 0);

	/* Save the timestamp for this log handle */
	current_log_tm = *tm;
	return f;
}

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

static int safe_fclose(FILE *f) {
	if (f != NULL && f != stderr && f != stdout) {
		return fclose(f);
	}
	return 0;
}

static int strjoin(char *dest, size_t n, char *left, char mid, char *right) {
	size_t llen = strlen(left);
	size_t rlen = strlen(right);
	size_t len = llen + 1 + rlen;
	if (len + 1 > n) {
		return -1;
	}
	memcpy(dest, left, llen);
	dest[llen] = mid;
	memcpy(dest+llen+1, right, rlen);
	dest[len] = '\0';
	return 0;
}

int stupid_log_init(char *directory, char *prefix, enum STUPID_LOG_ROLLOVER rollover) {
	char pathbuf[PATH_BUFSIZE];
	FILE *tmp;

	rollover_granularity = rollover;

	/* Check if the file paths will fit in our buffers.
	 * One extra char for the "/" and "\0" respectively. */
	if (strlen(directory) + strlen(prefix) + 1 + 1 > PATH_BUFSIZE) {
		errno = ENOMEM;
		return -1;
	}

	/* Check if the directory is writeable */
	strjoin(pathbuf, sizeof(pathbuf), directory, '/', ".sl4ctmp");
	tmp = fopen(pathbuf, "w");
	if (tmp == NULL) {
		return -1;
	}
	if (fputc('\n', tmp) == EOF) {
		fclose(tmp);
		remove(pathbuf);
		return -1;
	}
	fclose(tmp);
	remove(pathbuf);

	/* Save file prefix */
	strjoin(filenameprefix, sizeof(filenameprefix), directory, '/', prefix);
	return 0;
}

void stupid_log_close() {
	safe_fclose(logfile);
	logfile = NULL;
	return;
}

int stupid_log(const char *level, const char *format, ...) {
	time_t t;
	struct tm *tm;
	char tbuf[32];
	FILE *out;
	va_list args;
	int errsave = 0;

	t = time(NULL);
	tm = localtime(&t);
	tbuf[strftime(tbuf, sizeof(tbuf), "%F %T", tm)] = '\0';

	/* Get the log handler. If we're not writing to a file, still print to the
	 * std stream we got, but report the error later */
	out = stupid_log_handle(tm);
	if (out == stderr || out == stdout) {
		errsave = errno;
	}

	clearerr(out);

	/* Write the prefix */
	fprintf(out, "%s [%-5s] ", tbuf, level);

	/* Write the caller's arguments */
	va_start(args, format);
	vfprintf(out, format, args);
	va_end(args);

	/* Append new line */
	fputc('\n', out);

	if (errsave != 0) {
		/* Propagate errsave if we have one */
		errno = errsave;
		return -1;
	}
	if ((errsave = ferror(out)) != 0) {
		/* If the stream had an error, report that in errno.
		 * Returning -2 to distinguish that this errno is from the stream. */
		errno = errsave;
		return -2;
	}
	return 0;
}
