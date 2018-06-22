#include <stdio.h>
#include <pthread.h>

#define MAXN 1000000

double worker_output;

void *worker(void *arg){
	int p = MAXN/2;
	int flag = 1;
	if(!(p&1))
		p++;
	if((p/2)&1)
		flag = -1;

	for(; p <= MAXN; p += 2){
		worker_output += flag*1.0/p;
		flag *= -1;
	}

	return NULL;
}

double master_output;

void master(){
	int end = MAXN/2;
	int p = 1;
	int flag = 1;
	for(; p <= end; p += 2){
		master_output += flag*1.0/p;
		flag *= -1;
	}
}

int main(int argc, char const *argv[])
{
	pthread_t worker_tid;
	double ans = 0;

	pthread_create(&worker_tid, NULL, worker, NULL);
	master();	
	pthread_join(worker_tid, NULL);
	ans = master_output + worker_output;
	printf("master : %.8lf .\nworker : %.8lf .\nans : %.8lf .\n", master_output, worker_output, ans);

	return 0;
}