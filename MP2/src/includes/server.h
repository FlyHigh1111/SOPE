#pragma once
#include "./common.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_THREADS 10000
#define BUFF_SIZE 1024;
struct Arguments
{
    size_t nsecs;      //number of seconds
    int buffer_size;    //buffer size
    char *public_fifo; //public FIFO
};
struct ArgsThread
{
    pid_t pid; //process id of the program
    int fd_public_fifo; //file descriptor of the public FIFO
};

struct ArgsThreadSProd
{   int rid; 		// request id
	pid_t pid; 	    // process id
	pthread_t tid;	// thread id
	int tskload;    // task load
	int tskres;		// task result
    struct Queue queue; //armazem

};

struct ArgsThreadSCon
{  
    struct Queue queue; //armazem
};

struct Queue{
    int primeiro;//indice do primeiro elemento da fila(-1 se fila vazia )
    int ultimo;//indice do ultimo elemento da fila (-1 se fila vazia )


};

bool finish=false;

bool isNumeric(char num[]);
void sigAlrmHandlerS(int signum);