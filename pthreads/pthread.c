#include <stdio.h>
#include <pthread.h>
#define NUM_THREADS 4


void* hello(void* arg) {
	printf("Hello world from thread %d \n", *(int*)arg);
	return 0;
}

int main(void) {
	pthread_t tid[NUM_THREADS];
	int tNum[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; i++) {
		tNum[i] = i;
		pthread_create(&tid[i], NULL, hello, &tNum[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		if(pthread_join(tid[i], NULL) != 0) {
			printf("Thread %d failed to join successfully", i);
		}
	}

	return 0;
}
