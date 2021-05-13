#pragma once
#include "./common.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "./lib.h"
#include "./queue.h"


#define MAX_THREADS 10000

pthread_mutex_t lock;
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
    struct Message *armazem ; //armazem
    int nmax;//numero max delementos armazem

};

struct ArgsThreadSCon
{  
    struct Message *armazem; //armazem
    int nmax;
};

bool finish=false;

bool isNumeric(char num[]);
void sigAlrmHandlerS(int signum);