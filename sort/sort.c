#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define inf 0x3f3f3f3f

int arr[] = {9,8,7,6,5,4,3,2,1};
int size_arr;

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void *sort_part(void *arg)
{
    int i;
    int l = ((int *)arg)[0];
    int r = ((int *)arg)[1];
    for(; l < r; l++) {
        int i = l, flagi = l;
        int min = arr[l];
        for(; i < r; i++) {
            if(arr[i] < min) {
                flagi = i;
                min = arr[i];
            }
        }
        swap(&arr[flagi], &arr[l]);
    }
}

void merge(int p, int q, int r)
{
    int n1 = q-p;
    int n2 = r-q+1;
    int L[n1+1], R[n2+1];
    int i, j;
    for(i=1; i<=n1; i++)
        L[i-1] = arr[p+i-1];
    for(i=1; i<=n2; i++)
        R[i-1] = arr[q+i-1];
    L[n1] = R[n2] = inf;
    i = j = 0;
    int k;
    for(k=p; k<=r; k++) {
        if(L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
    }
}

int main(int argc, char const *argv[])
{
    size_arr = sizeof(arr)/sizeof(int);
    pthread_t thread;
    int a[] = {size_arr/2, size_arr};
    int b[] = {0, size_arr/2};
    sort_part((void *)b);

    pthread_create(&thread, NULL, sort_part, (void *)a);
    pthread_join(thread, NULL);
    merge(0, (size_arr-1)/2, size_arr-1);
    int i;
    for(i = 0; i <size_arr; i++) {
        printf("%d", arr[i]);
        if(i != size_arr-1)
            printf(" ");
        else
            printf("\n");
    }
    return 0;
}