#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>


#define FILE_NAME "test_case"
#define PROBLEM_SIZE 1000
#define MIN_SIZE 100
#define NUM_THREAD 100
using namespace std;

vector<int> arr(PROBLEM_SIZE, 0);

struct Parameter{
    vector<int>::iterator low, high;
};

void * runner(void * p);

void insertSort(Parameter * p);


void initializize()
{
    for(int i =0; i < PROBLEM_SIZE; i++)
        arr[i] = i;
    random_shuffle(arr.begin(), arr.end());
    ofstream out(FILE_NAME, ofstream::out);
    if(!out) {
        cout << "open file failed\n";
        return;
    }
    for(auto it : arr)
    {
        out << it << ' ';
    }
    out << endl;
    out.close();
}

void insertSort(vector<int>::iterator low, vector<int>::iterator high)
{
    vector<int>::iterator min;
    int tmp;
    for(auto i = low; i <= high; i++)
    {
        min = i;
        for(auto j = i; j <= high; j++)
            min = (*j < *min)? j : min;
        tmp = *i; *i = *min; *min = tmp;
    }
}

void * runner(void * pr)
{
    Parameter * p = (Parameter*)pr;

    if((p->low + MIN_SIZE) < p->high){
        insertSort(p->low, p->high);
    }
    else{
        if(p->low >= p->high) return NULL;
        auto left = p->low, right = p->high;
        int k = *left;
        while(left < right){
            while(left < right && *right > k) right--;
            if(left < right)
            {
                int tmp = *left; *left = *right; *right = tmp;
            }
            while(left < right && *left <= k) left++;
            if(left < right)
            {
                int tmp = *left; *left = *right; *right = tmp;
            }
        }
        Parameter p1, p2;
        pthread_t t1, t2;
        pthread_attr_t attr1, attr2;


        pthread_attr_init(&attr1);
        p1.low = p->low;
        p1.high = left - 1;
        pthread_create(&t1, &attr1, runner, (void*)&p1);

        pthread_attr_init(&attr2);
        p2.low = right + 1;
        p2.high = p->high;
        pthread_create(&t2, &attr2, runner, (void*)&p2);
        
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
    }
}


int main(int argc, char const *argv[])
{
    initializize();
    Parameter p;
    p.low = arr.begin(); p.high = arr.end() - 1;
    pthread_t pth;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&pth, &attr, runner, (void*)&p);

    pthread_join(pth, NULL);
    ofstream out(FILE_NAME, ofstream::app);
    if(!out) {
        cout << "open file failed\n";
        return 0;
    }
    out << endl << "sorted list:\n";
    for(auto it : arr)
    {
        out << it << ' ';
    }
    out << endl;
    out.close();
    return 0;
}