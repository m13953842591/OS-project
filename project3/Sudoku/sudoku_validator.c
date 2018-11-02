#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 11
#define FILE_NAME "test_case"

int *sudoku[9];
int checkRow[9][9], checkCol[9][9], checkGrid[9][9];
FILE *stream;

struct param{
    int id;
    int valid;
};

struct param validator[NUM_THREADS];

void *runner(struct param * p);/* threads call this function */


int main(int argc, char const *argv[])
{

    pthread_t workers[NUM_THREADS]; /* the thread identifier */
    pthread_attr_t attr[NUM_THREADS]; /* set of thread attributes */

    /* initialization */
    if((stream = fopen(FILE_NAME, "r")) == NULL)
    {
        printf("open file failed\n");
        return 0;
    }
    /* input sudoku from file */
    for(int i = 0; i < 9; i++)
    {
        sudoku[i] = (int*) malloc(9*sizeof(int));
        for(int j = 0; j < 9; j++)
        {
            checkCol[i][j] = checkRow[i][j] = checkGrid[i][j] = 0;
            fscanf(stream, "%d", &sudoku[i][j]);
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }

    /* multithreading */
    for(int i = 0; i < NUM_THREADS; i++)
    {
        /* set the default attributes of the thread */
        pthread_attr_init(&attr[i]);
        validator[i].id = i;
        /* create the thread */
        pthread_create(&workers[i], &attr[i], runner, &validator[i]);
    }
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        /* wait for the thread to exit */
        pthread_join(workers[i], NULL);
    }
   
    for(int i = 0; i < NUM_THREADS; i++)
    {
        if(!validator[i].valid){
            printf("invalid sudoku!\n");
            return 0;
        }
    }
        
    printf("valid sudoku!\n");

    return 0;
}

/*  The thread will execute in this function  */

void *runner(struct param *p)
{
    if(p->id == 0) /* check row */
        for(int i = 0; i < 9; i++)
            for(int j = 0; j < 9; j++)
            {
                if(checkRow[i][sudoku[i][j] - 1] == 1)
                {
                    p->valid = 0;
                    printf("find invalid row %d\n", i);
                    pthread_exit(0);
                }
                checkRow[i][sudoku[i][j] - 1] = 1;
            }
    else if(p->id == 1) /* check column */
        for(int j = 0; j < 9; j++)
            for(int i = 0; i < 9; i++)
            {
                if(checkCol[j][sudoku[i][j] - 1] == 1)
                {
                    p->valid = 0;
                    printf("find invalid column %d\n", j);
                    pthread_exit(0);
                }
                checkCol[j][sudoku[i][j] - 1] = 1;
            }
    else {
        int grid = p->id - 2;
        int start_i = grid / 3 * 3, start_j = grid % 3 * 3;
        for(int i = start_i; i < start_i + 3; i++)
            for(int j = start_j; j < start_j + 3; j++)
            {
                if(checkGrid[grid][sudoku[i][j] - 1] == 1)
                {
                    p->valid = 0;
                    printf("find invalid grid %d\n", grid);
                    pthread_exit(0);
                }
                checkGrid[grid][sudoku[i][j] - 1] = 1;
            }
    }
    p->valid = 1;
    pthread_exit(0);
}