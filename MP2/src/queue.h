#pragma once
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include "./server.h"

struct Queue{
    int primeiro;//indice do primeiro elemento da fila(-1 se fila vazia )
    int ultimo;//indice do ultimo elemento da fila (-1 se fila vazia )
};

struct Queue queue;

void initqueue(struct Queue *q);
bool queueIsEmpty(struct Queue *q);
bool queueIsFull(struct Queue *q,int nmax);
bool pushbackqueue(struct Queue *q,struct Message armazem[],struct Message new_message,int nmax);
bool topQueue(struct Queue *q,struct Message armazem[],struct Message *top);
bool popQueue(struct Queue *q,struct Message armazem[],int nmax);