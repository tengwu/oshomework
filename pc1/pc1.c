#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 4
#define ITEM_COUNT (CAPACITY*2)
char buffer[3][CAPACITY];
int in[3], out[3];

pthread_mutex_t mutex[3];
pthread_cond_t wait_empty_buffer[3];
pthread_cond_t wait_full_buffer[3];

int buffer_is_empty(int bufferid){
	return in[bufferid] == out[bufferid];
}

int buffer_is_full(int bufferid){
	return (in[bufferid]+1)%CAPACITY == out[bufferid];
}

char get_item(int bufferid){
	char item;

	item = buffer[bufferid][out[bufferid]];
	out[bufferid] = (out[bufferid]+1)%CAPACITY;

	return item;
}

void put_item(int bufferid, char item){
	buffer[bufferid][in[bufferid]] = item;
	in[bufferid] = ((in[bufferid]+1)%CAPACITY);
}

void print()
{
	int ii, jj;
	for(ii=1;ii<=2;ii++){
		for(jj=0;jj<4;jj++){
			printf("%c ", buffer[ii][jj]);
		}
		printf("\n");
	}
}

void *consume(void *arg){
	int i;
	char item;

	for(i = 0; i <ITEM_COUNT; i++){
		printf("consumer\n");
		// consume and compute access buffer2 mutually exclusively
		pthread_mutex_lock(&mutex[2]);
		// print();
		while(buffer_is_empty(2))
			// wait until buffer2 is full
			pthread_cond_wait(&wait_full_buffer[2], &mutex[2]);

		item = get_item(2);
		printf("		consume item: %c\n", item);

		// say that buffer2 is empty
		pthread_cond_signal(&wait_empty_buffer[2]);
		pthread_mutex_unlock(&mutex[2]);
	}
	return NULL;
}

void *produce(void *arg){
	int i;
	char item;

	for(i = 0; i < ITEM_COUNT; i++){
		printf("producer\n");
		// produce and compute access buffer1 mutually exclusively
		pthread_mutex_lock(&mutex[1]);
		// print();
		while(buffer_is_full(1))
			pthread_cond_wait(&wait_empty_buffer[1], &mutex[1]);

		item = 'a'+i;
		put_item(1, item);
		printf("produce item: %c\n", item);

		// say that buffer1 is full
		pthread_cond_signal(&wait_full_buffer[1]);
		pthread_mutex_unlock(&mutex[1]);
	}
	return NULL;
}

void *compute(void *arg){
	int i;
	char item;

	for(i = 0; i < ITEM_COUNT; i++){
		printf("computer\n");
		// compute and produce and comsume access buffer1 mutually exclusively
		pthread_mutex_lock(&mutex[1]);
		pthread_mutex_lock(&mutex[2]);
		// print();
		while(buffer_is_empty(1)){
			pthread_cond_wait(&wait_full_buffer[1], &mutex[1]);
		}
		while(buffer_is_full(2)){
			pthread_cond_wait(&wait_empty_buffer[2], &mutex[2]);
		}

		item = get_item(1);
		item -= 'a'-'A';
		put_item(2, item);
		printf("	compute: %c -> %c\n", item+'a'-'A', item);

		pthread_cond_signal(&wait_empty_buffer[1]);
		pthread_cond_signal(&wait_full_buffer[2]);

		pthread_mutex_unlock(&mutex[1]);
		pthread_mutex_unlock(&mutex[2]);
	}
}

int main()
{
	pthread_t producer, computer, consumer;
	pthread_mutex_init(&mutex[1], NULL);
	pthread_mutex_init(&mutex[2], NULL);
	pthread_cond_init(&wait_full_buffer[1], NULL);
	pthread_cond_init(&wait_full_buffer[2], NULL);
	pthread_cond_init(&wait_empty_buffer[1], NULL);
	pthread_cond_init(&wait_empty_buffer[2], NULL);

	pthread_create(&producer, NULL, produce, NULL);
	pthread_create(&computer, NULL, compute, NULL);
	pthread_create(&consumer, NULL, consume, NULL);

	pthread_join(producer, NULL);
	pthread_join(computer, NULL);
	pthread_join(consumer, NULL);

	return 0;
}