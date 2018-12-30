/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3
#define TRUE 1


// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue

task queue[QUEUE_SIZE];
int front = 0, back = 0;
// the worker bee
pthread_t bee[NUMBER_OF_THREADS];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    if((back + 1)%QUEUE_SIZE == front){
	    /* queue is full */
		printf ("[error] enqueue but queue is full\n");
		return 1;
    }
    queue[back].data = t.data;
    queue[back].function = t.function;
    back = (back + 1) % QUEUE_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    task worktodo;
    if (front == back) {
        /* queue is empty */
            printf ("[error] dequeue but task queue is empty!");
        } else {
            worktodo.function = queue[front].function;
            worktodo.data = queue[front].data;
            front = (front + 1) % QUEUE_SIZE;
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    task w;
    // execute the task
    while(TRUE)
    {
        w = dequeue();
        execute(w.function, w.data);
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task worktodo;
    worktodo.function = somefunction;
    worktodo.data = p;
    enqueue(worktodo);
    return 0;
}

// initialize the thread pool
void pool_init(void)
{
    sem_init(&full, 0, 0); /* initialized to 0 */
    sem_init(&empty, 0, QUEUE_SIZE - 1); /* initialized to 10 */
    pthread_mutex_init(&mutex, NULL);
    front = 0; back = 0;
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_create(&bee[i],NULL,worker,NULL);
    }

}

// shutdown the thread pool
void pool_shutdown(void)
{
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_cancel(bee[i]);
    }
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_join(bee[i], NULL);
    }
}
