#pragma once
#include "./common.h"

struct Queue
{
    int first; //index of the first element of the queue (-1 if queue is empty)
    int last;  //index of the last element of the queue (-1 if queue is empty)
};

struct Queue queue;

void initQueue(struct Queue *q);
bool queueIsEmpty(struct Queue *q);
bool queueIsFull(struct Queue *q, int nmax);
bool pushBackQueue(struct Queue *q,struct Message cloud[],struct Message new_message,int nmax);
bool topQueue(struct Queue *q,struct Message cloud[],struct Message *top);
bool popQueue(struct Queue *q,struct Message cloud[],int nmax);
