#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 4
#define ITEM_COUNT (CAPACITY * 2)

int buffer[3][CAPACITY];
int in[3];
int out[3];

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

sema_t mutex_sema[3];
sema_t empty_buffer_sema[3];
sema_t full_buffer_sema[3];

int buffer_is_empty(int bufferid)
{
    return in[bufferid] == out[bufferid];
}

int buffer_is_full(int bufferid)
{
    return ((in[bufferid] + 1) % CAPACITY) == out[bufferid];
}

int get_item(int bufferid)
{
    int item;

    item = buffer[bufferid][out[bufferid]];
    out[bufferid] = (out[bufferid] + 1) % CAPACITY;
    return item;
}

void put_item(int bufferid, int item)
{
    buffer[bufferid][in[bufferid]] = item;
    in[bufferid] = (in[bufferid] + 1) % CAPACITY;
}


void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while (sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        sema_wait(&full_buffer_sema[2]);
        sema_wait(&mutex_sema[2]);

        item = get_item(2);
        printf("		consume item: %c\n", item); 

        sema_signal(&mutex_sema[2]);
        sema_signal(&empty_buffer_sema[2]);
    }

    return NULL;
}

void *produce(void *arg)
{	
	int i;
	int item;

	for(i = 0; i < ITEM_COUNT; i++){
		sema_wait(&empty_buffer_sema[1]);
		sema_wait(&mutex_sema[1]);

		item = 'a'+i;
		put_item(1, item);
		printf("produce item: %c\n", item);

		sema_signal(&mutex_sema[1]);
		sema_signal(&full_buffer_sema[1]);
	}

	return NULL;
}

void *compute(void *arg)
{
	int i;
	int item;

	for(i = 0; i < ITEM_COUNT; i++){
		sema_wait(&full_buffer_sema[1]);
		sema_wait(&mutex_sema[1]);
		sema_wait(&empty_buffer_sema[2]);
		sema_wait(&mutex_sema[2]);

		item = get_item(1);
		item += 'A'-'a';	
		put_item(2, item);
		printf("	compute item: %c -> %c\n", item-'A'+'a', item);

		sema_signal(&mutex_sema[1]);
		sema_signal(&mutex_sema[2]);
		sema_signal(&empty_buffer_sema[1]);
		sema_signal(&full_buffer_sema[2]);
	}

	return NULL;
}

int main()
{
	pthread_t producer, computer, consumer;
	sema_init(&mutex_sema[1], 1);
	sema_init(&mutex_sema[2], 1);
	sema_init(&empty_buffer_sema[1], CAPACITY - 1);
	sema_init(&empty_buffer_sema[2], CAPACITY - 1);
	sema_init(&full_buffer_sema[1], 0);
	sema_init(&full_buffer_sema[2], 0);

	pthread_create(&producer, NULL, produce, NULL);
	pthread_create(&computer, NULL, compute, NULL);
	pthread_create(&consumer, NULL, consume, NULL);

	pthread_join(producer, NULL);
	pthread_join(computer, NULL);
	pthread_join(consumer, NULL);

    return 0;
}