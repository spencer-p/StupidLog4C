#include "stupidlog4c.h"

static char filenameprefix[256] = {0};
static _Thread_local FILE *logfile = NULL;

static FILE *stupid_log_make_handle() {
	FILE *f = fopen(filenameprefix, "a");
	if (f == NULL) {
		return stderr;
	}
	setlinebuf(f);
	return f;
}

int stupid_log_init(char *directory, char *prefix) {
	char pathbuf[256];
	FILE *tmp;

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
	if (logfile != NULL && logfile != stderr && logfile != stdout) {
		int rc = fclose(logfile);
		logfile = NULL;
		return rc;
	}
	return 0;
}

FILE *stupid_log_handle() {
	if (logfile == NULL) {
		logfile = stupid_log_make_handle();
	}
	return logfile;
}
