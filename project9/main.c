#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CMD 20

struct process{
    char pid[MAX_CMD];
    int base;
    int limit;
    struct process *next;
};
struct process * new_process(const char* pid, int base, int limit, struct process *next);
struct process * first_fit(int limit);
struct process * best_fit(int limit);
struct process * worst_fit(int limit);
void allocate(const char * pid, int limit, char opt);
void release_memory(const char * pid);
void show_status(void);
void compaction(void);
void clear(void);


struct process *tail, * head;
int vol = 10000000;
int main(int argc, char const *argv[])
{
    
    char cmd[MAX_CMD], pid[MAX_CMD], opt;
    int limit;
    if(argc < 2)
        printf("[warning] invalid number of argument, set default to 10000000\n");
    else vol = atoi(argv[1]);
    tail = new_process("tail", vol, 0, NULL);
    head = new_process("head", 0, 0, tail);

    while(true)
    {
        printf("allocator(enter q to quit)>");
        scanf("%s", cmd);
        if(strcmp(cmd, "q") == 0)
            break;
        if(strcmp(cmd, "RQ") == 0)
        {
            scanf("%s %d %c", pid, &limit, &opt);
            allocate(pid, limit, opt);
        }
        else if(strcmp(cmd, "RL") == 0)
        {
            scanf("%s", pid);
            release_memory(pid);
        }
        else if(strcmp(cmd, "C") == 0)
        {
            compaction();
        }
        else if(strcmp(cmd, "STAT") == 0)
        {
            show_status();
        }
        else{
            printf("unknow command\n");
        }
        fflush(stdin);
    }
    printf("exiting\n");
    clear();
    return 0;
}

struct process * new_process(const char* pid, int base, int limit, struct process *next)
{
    struct process * p = (struct process *) malloc (sizeof (struct process));
    p->base = base;
    p->limit = limit;
    strcpy(p->pid, pid);
    p->next = next;
    return p;
}

void allocate(const char * pid, int limit, char opt)
{
    // printf("pid = %s, limit = %d, opt = %c\n", pid, limit, opt);
    struct process *ptr = NULL;
    switch(opt){
        case 'F':
            ptr = first_fit(limit);
            break;
        case 'B':
            ptr = best_fit(limit);
            break;
        case 'W':
            ptr = worst_fit(limit);
            break;
        default:
            printf("[error] invalid mode \n");
    }
    if(ptr != NULL)
        ptr->next = new_process(pid, ptr->base + ptr->limit, limit, ptr->next);
    else printf("allocation failed\n");
}

struct process * first_fit(int limit)
{
    struct process *p = head;
    while(p->next != NULL)
    {
        if(p->base + p->limit + limit < p->next->base)
            break;
        p = p->next;
    }
    return p;
}

struct process * best_fit(int limit)
{
    struct process *p = head, * best_p = NULL;
    int temp = 0, min_vol = vol + 1;
    while(p->next != NULL)
    {
        temp = p->next->base - (p->base + p->limit);
        if(temp >= limit && temp < min_vol){
            best_p = p;
            min_vol = temp;
        }
        p = p->next;
    }
    return best_p;
}

struct process * worst_fit(int limit)
{
    struct process *p = head, * worst_p = NULL;
    int temp = 0, max_vol = 0;
    while(p->next != NULL)
    {
        temp = p->next->base - (p->base + p->limit);
        if(temp >= limit && temp > max_vol){
            worst_p = p;
            max_vol = temp;
        }
        p = p->next;
    }
    return worst_p;
}

void release_memory(const char * pid)
{
    struct process * p = head;
    bool flag = false;
    while(p->next != NULL)
    {
        if(strcmp(pid, p->next->pid) == 0)
        {
            struct process *q = p->next;
            p->next = q->next;
            free(q);
            flag = true;
            break;
        }
        p = p->next;
    }
    
}

void show_status(void)
{
    struct process *p = head->next;
    int start = 0;
    while(p != NULL)
    {
        if(start < p->base)
            printf("Address [%d: %d] Unused\n", start, p->base);
        if(p->next != NULL)
        {
            start = p->base + p->limit - 1;
            printf("Address [%d: %d] Process %s\n", p->base, p->base + p->limit, p->pid);
            ++start;
        }
        p = p->next;
    }
}

void compaction(void)
{
    struct process *p = head;
    while(p->next != NULL)
    {
        p->next->base = p->base + p->limit;
        p = p->next;
    }
}

void clear(void)
{
    struct process *p = head;
    while(p->next != NULL)
        release_memory(p->next->pid);
    free(head);
}