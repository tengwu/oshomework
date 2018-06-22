#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MAXN 1000000
#define N 2

double res[N+1];

int min(int a, int b){
	return a < b ? a : b;
}

void *compute(void *arg){
	int n = *(int *) arg;
	int count = (MAXN + N - 1)/N;
	int p = (n-1)*count+1;
	int end = min(n*count, MAXN);
	int flag = 1;
	if(p&1 == 0)
		p++;
	if((p/2)&1 == 1)
		flag = -1;
	for(; p <= end; p += 2){
		res[n] += flag*1.0/p;
		flag *= -1;
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t thread[N+1]; // not use worker[0]
	double ans = 0;
	memset(res, 0, sizeof(res));

	int i;
	for(i = 1; i <= N; i++){
		pthread_create(&thread[i], NULL, compute, &i);
		pthread_join(thread[i], NULL);
	}

	for(i = 1; i <= N; i++){
		ans += res[i];
	}

	printf("ans = %.8lf .\n", ans);

	return 0;
}