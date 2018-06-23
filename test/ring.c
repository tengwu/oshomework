#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#define NR_CPU 5
#define LIMIT 10

pthread_mutex_t mutex;	
pthread_cond_t wait[LIMIT];

int cnt,id[NR_CPU];

int get_x(int x){
	return cnt == x;
}
void *send(void *arg){
	int ID = *(int *)arg;
	int need = ID-1;
	while(1){
		pthread_mutex_lock(&mutex);
		while(!get_x(need)){
			pthread_cond_wait(&wait[need], &mutex);
		}
		printf("T%d: %d  ",ID, need);
		need += NR_CPU;
		cnt ++;
		printf("need=%d  cnnt=%d\n",need,cnt);
		pthread_cond_signal(&wait[cnt]);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main() {
	pthread_t t[NR_CPU];
	int i;
	for(i=0;i<LIMIT;i++){
		pthread_cond_init(&wait[i], NULL);
	}
	cnt = 0;
	for(int i=0;i<NR_CPU;i++){
		id[i] = i+1;
		pthread_create(&t[i],NULL,send,&id[i]);
	}
	for(int i=0;i<NR_CPU;i++){
		pthread_join(t[i],NULL);
	}
	return 0;
}