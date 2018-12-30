#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMBER_OF_CUSTOMERS 5 
#define NUMBER_OF_RESOURCES 4
const char* filename = "a.in";
char buffer[100];
int cur_num;
int request[NUMBER_OF_RESOURCES];
/* the available amount of each resource */ 
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */ 
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */ 
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]); 
void release_resources(int customer_num, int release[]);

int is_allocatable(int available[], int need[]){
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if(need[i] > available[i])
            return 0;
    }
    return 1;
}

int is_safty()
{
    int available_copy[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];

    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        available_copy[i] = available[i];
        finish[i] = 0;
    }
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if(!finish[i] && is_allocatable(available_copy, need[i]))
        {
            finish[i] = 1;
            for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
            {
                available_copy[j] += allocation[i][j];
            }
        }
    }
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if(!finish[i])
            return 0;
    }
    return 1;
}


int request_resources(int customer_num, int request[])
{
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if(request[i] > available[i] ||
             maximum[customer_num][i] < request[i] + allocation[customer_num][i])
            return 0;
    }
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }
    if(!is_safty())
    {
        for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
        {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return 0;
    }
    return 1; 
}

void release_resources(int customer_num, int release[])
{
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if(release[i] > allocation[customer_num][i])
            return;
    }
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
        available[i] += release[i];
    }
    // return 1;
}

void print_array(int array[])
{
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}
int main(int argc, char const *argv[])
{
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        // printf("argv[%d] = %s\n", i + 1, argv[i + 1]);
        available[i] = atoi(argv[i+1]);
    }
    FILE * in;
    char * tmp;
    if((in = fopen(filename, "r")) == NULL)
    {
        printf("open file error\n");
    }
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
        {
            fscanf(in, "%d", &maximum[i][j]);
            need[i][j] = maximum[i][j];
            // printf("%d ", maximum[i][j]);
        }
        // printf("\n");
    }
    while(1)
    {
        printf("\nplease enter buffer('q' to quit)>> ");
        fflush(stdin);
        scanf("%s", buffer);
        if(buffer[0] == 'q')
            break;
        if(buffer[0] == '*')
        {
            printf("available:\n");
            print_array(available);
            printf("maximum:\n");
            for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
                print_array(maximum[i]);
        
            printf("allocation:\n");
            for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
                print_array(allocation[i]);
            
            printf("need\n");
            for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
                print_array(need[i]);
        }
        else{
            printf("command = [%s]\n", buffer);
            scanf("%d", &cur_num);
            printf("cur_num = %d\n", cur_num);
            for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
            {
                scanf("%d", &request[i]);
            }
            if(buffer[1] == 'Q'){
                if(request_resources(cur_num, request))
                {
                    printf("request succeed\n");
                }
                else printf("request failed\n");
            }
            else if(buffer[1] == 'L'){
                release_resources(cur_num, request);
            }
            else {
                printf("buffer not found\n");
            }
        }
    }
    

    return 0;
}
