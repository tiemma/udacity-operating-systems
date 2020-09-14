#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define NUM_THREADS 5
#define BUF_SIZE 3

int buf[BUF_SIZE];
int tNum[BUF_SIZE];

pthread_t reader_threads[NUM_THREADS];
pthread_t writer_threads[NUM_THREADS];

int count = 0;
int readers = 0;

pthread_mutex_t mux;
pthread_cond_t read_cond, write_cond;

void* reader(void* arg) {
	if(count < 0) { //underflow
		printf("Unknown error occurred\n");
		exit(1);
	}

	pthread_mutex_lock(&mux);
	while(count < 0) {
		pthread_cond_wait(&read_cond, &mux);
	}
	readers = readers + 1;
	printf("Read value %d from index %d\nReaders: %d\n", buf[count], count, readers);
	buf[count] = 0;
	if(count > 0) {
		count = count - 1;
	}
	readers = readers - 1;
	pthread_mutex_unlock(&mux);

	if(count == 0) {
		pthread_cond_signal(&write_cond);
	}
}

void* writer(void* arg) {
	int* writer_idx = (int*)arg;
	if(count == BUF_SIZE) { //overflow
		printf("Number of writes greater than buffer size\n");
		exit(1);
	}

	pthread_mutex_lock(&mux);
	while(readers > 0 || count == BUF_SIZE) {
		pthread_cond_wait(&write_cond, &mux);
	}
	buf[count] = count + 1;
	printf("Wrote value %d to index %d\nReaders: %d\tWriter Index: %d\n", count + 1, count, readers, *writer_idx);
	count = count + 1;
	pthread_mutex_unlock(&mux);

	if(count == BUF_SIZE - 1) {
		pthread_cond_signal(&read_cond);
	}
}

int main() {
	// Init the mutex
	pthread_mutex_init(&mux, NULL);

	// Create the writers
	for(int i = 0; i < NUM_THREADS; i++) {
		tNum[i] = i;
		if(pthread_create(&writer_threads[i], NULL, writer, (int*)&tNum[i]) != 0){
			printf("Error occurred whilst creating writer thread %d\n", i);
			exit(1);
		}
	}


	// Create the readers
	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_create(&reader_threads[i], NULL, reader, NULL) != 0){
			printf("Error occurred whilst creating reader thread %d\n", i);
			exit(1);
		}
	}

	// Join the writer threads
	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_join(writer_threads[i], NULL) != 0) {
			printf("Error on joining writer thread %d to main thread\n", i);
			exit(1);
		}
	}

	// Join the reader threads
	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_join(reader_threads[i], NULL) != 0) {
			printf("Error on joining reader thread %d to main thread\n", i);
			exit(1);
		}
	}

	return 0;
}
