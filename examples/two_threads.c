#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../stupidlog4c.h"

void *logstuff(void *arg) {
	int threadnum = *((int*)arg);
	int i;

	for (i = 0; i < 10; i++) {
		/* Log calls are successful in any thread as long as stupid_log_init was
		 * previously called */
		stupid_log_info("From thread %d, i equals %d", threadnum, i);
	}

#ifndef STUPID_LOG_USE_PTHREAD
	/*
	 * If we don't have pthread keys for the file streams, we have to close the
	 * logs ourselves.
	 * If we compiled with STUPID_LOG_USE_PTHREAD and linked against pthread,
	 * then we can skip this -- the data will be cleaned up automatically.
	 */
	stupid_log_close();
#endif
	return NULL;
}

int main() {
	if (stupid_log_init("/tmp", "multi_thread_log", STUPID_LOG_HOURLY) < 0) {
		perror("log_init");
		exit(1);
	}

	stupid_log_trace("Starting two threads");
	
	pthread_t one, two;
	pthread_create(&one, NULL, logstuff, &(int){1});
	pthread_create(&two, NULL, logstuff, &(int){2});

	pthread_join(one, NULL);
	pthread_join(two, NULL);

	stupid_log_trace("Exiting");
	stupid_log_close();
	return 0;
}
