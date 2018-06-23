#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_N 5

int buffer[THREAD_N+1];
pthread_mutex_t mutex;
pthread_cond_t wait_update_data_buffer[THREAD_N+1];

int no_data_in_buffer(int bufferid, int cur){
	return cur == buffer[bufferid];
}

void *compute(void *arg){
	int id = *(int *)arg;
	int cur = -1;
	if(id == 1)
		buffer[id] = 0;
	while(1){
		pthread_mutex_lock(&mutex);
		int next_id = id%THREAD_N+1;
		while(no_data_in_buffer(id, cur))
			pthread_cond_wait(&wait_update_data_buffer[id], &mutex);

		cur = buffer[id];
		buffer[next_id] = cur+1;

		printf("tid: %d, next_id: %d, calculate: %d\n", id, next_id, cur);

		pthread_cond_signal(&wait_update_data_buffer[next_id]);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

int main()
{
	int i;
	int ids[THREAD_N+1];

	pthread_t thread[THREAD_N+1];
	for(i = 1; i <= THREAD_N; i++) buffer[i] = -1;
	for(i = 1; i <= THREAD_N; i++){
		pthread_cond_init(&wait_update_data_buffer[i], NULL);
	}
	for(i = 1; i <= THREAD_N; i++){
		ids[i] = i;
		pthread_create(&thread[i], NULL, compute, (void *)&ids[i]);
	}
	for(i = 1; i <= THREAD_N; i++){
		pthread_join(thread[i], NULL);
	}

	return 0;
}