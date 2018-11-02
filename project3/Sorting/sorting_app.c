#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define FILE_NAME "test_case"
#define NUM_THREAD 2
int num, *arr;
FILE *stream;
struct Param{
    int * arr;
    int len;
};
struct Param p[NUM_THREAD];

void * runner(struct Param * p);

int main(int argc, char const *argv[])
{
    pthread_t workers[NUM_THREAD];
    pthread_attr_t attr[NUM_THREAD];

    if((stream = fopen(FILE_NAME, "r")) == NULL)
    {
        printf("open file failed\n");
        return 0;
    }
    /* input sudoku from file */
    fscanf(stream, "%d", &num);
    arr = (int*) malloc(num * sizeof(int));
    for(int i = 0; i < num; i++)
    {
        fscanf(stream, "%d", &arr[i]);
    }
    int fraglen = num / NUM_THREAD;
    printf("fraglen = %d\n", fraglen);
    for(int i = 0; i < NUM_THREAD; i++)
    {
        pthread_attr_init(&attr[i]);
        p[i].arr = arr + fraglen * i;
        p[i].len = fraglen;
        if(i == NUM_THREAD - 1 && fraglen * NUM_THREAD < num) 
        p[NUM_THREAD - 1].len = num - fraglen * (NUM_THREAD - 1);
        
        pthread_create(&workers[i], &attr[i], runner, &p[i]);
    }
    
    for(int i = 0; i < NUM_THREAD; i++)
    {
        pthread_join(workers[i], NULL);
    }

    int sorted[num], f = 0, s = 0;
    for(int i = 0; i < num; i++)
    {
        if(f < p[0].len && s < p[1].len){
            sorted[i] = (p[0].arr[f] < p[1].arr[s])? p[0].arr[f++] : p[1].arr[s++];
        }
        else if(f < p[0].len)
           sorted[i] = p[0].arr[f++];
        else sorted[i] = p[1].arr[s++];
    }

    for(int i = 0; i < num; i++)
    {
        printf("%d ", sorted[i]);
    }
    printf("\n");
    free(arr);
    return 0;
}

void* runner(struct Param * x)
{
    int min_j, tmp;
    for(int i = 0; i < x->len; i++)
    {
        min_j = i;
        for(int j = i; j < x->len; j++)
        {
            if(x->arr[j] < x->arr[min_j])
                min_j = j;
        }
        tmp = x->arr[i];
        x->arr[i] = x->arr[min_j];
        x->arr[min_j] = tmp;
    }
}