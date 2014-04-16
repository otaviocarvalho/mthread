#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"
#include <time.h>

#define MTHREAD_STACK 1024*1024

struct timespec time_diff(struct timespec start, struct timespec end){
	struct timespec temp;

	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
    else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}

	return temp;
}

void timespec_test(){
    struct timespec time1, time2;
    int i, temp;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
	for (i = 0; i< 242000000; i++)
		temp+=temp;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time2);

    printf("Thread execution time in seconds: %lf\n", (double) time_diff(time1,time2).tv_sec);
    printf("Thread execution time in nanoseconds: %lf\n", (double) time_diff(time1,time2).tv_nsec);
}

int mcreate (void (*start_routine)(void*), void *arg){
    return 0;
}
int myield(void){
    return 0;
}

int mjoin(int tid){
    return 0;
}

int mmutex_init(mmutex_t *m){
    return 0;
}

int mlock (mmutex_t *m){
    return 0;
}

int munlock (mmutex_t *m){
    return 0;
}
