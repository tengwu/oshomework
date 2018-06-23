#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAXN 1000000
#define N 100

int index_of_thread[N+1];

int min(int a, int b)
{
    return a < b ? a : b;
}

void *compute(void *arg)
{

    double* result;
    result = (double *)malloc(sizeof(double));
    *result = 0;

    int n = *(int *)arg;
    int count = (MAXN + N - 1)/N;
    int p = (n-1)*count+1;
    int end = min(n*count, MAXN);
    int flag = 1;
    if(p&1 == 0)
        p++;
    if((p/2)&1 == 1)
        flag = -1;
    for(; p <= end; p += 2) {
        *result += flag*1.0/p;
        flag *= -1;
    }

    return result;
}

int main(int argc, char const *argv[])
{
    pthread_t thread[N+1]; // not use worker[0]
    double ans = 0;

    int i;
    for(i = 1; i <= N; i++) {
        index_of_thread[i] = i;
        pthread_create(&thread[i], NULL, compute, index_of_thread+i);
    }

    for(i = 1; i <= N; i++) {
        double *result = 0;
        pthread_join(thread[i], (void **)&result);
        ans += *result;
        free(result);
    }

    printf("ans = %.8lf .\n", ans);

    return 0;
}