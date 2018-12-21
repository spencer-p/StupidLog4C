#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../stupidlog4c.h"

void *logstuff(void *arg) {
	int threadnum = *((int*)arg);

	for (int i = 0; i < 10; i++) {
		stupid_log_info("From thread %d, i equals %d", threadnum, i);
	}

	stupid_log_close();
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
}
