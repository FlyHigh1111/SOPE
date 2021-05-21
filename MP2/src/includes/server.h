#pragma once

#include "common.h"

#define MAX_THREADS 10000
#define SERVER -1

pthread_mutex_t lock;
bool finish=false;
bool finishCons=false;
int errno;

struct Arguments
{
    size_t nsecs;          //number of seconds
    int buffer_size;       //buffer size
    char public_fifo[100]; //public FIFO
};

struct ArgsThreadsProducer
{   int rid; 		// request id
	pid_t pid; 	    // process id
	pthread_t tid;	// thread id
	int tskload;    // task load
	int tskres;		// task result
    struct Message *cloud ;
    int nmax; //maximum number of elements in cloud

};

struct ArgsThreadsConsumer
{  
    struct Message *cloud;
    int nmax;
};

bool isNumeric(char num[]);
void ParseArguments(int argc, char *argv[], struct Arguments *args);
void sigAlrmHandlerS(int signum);
void* ThreadHandlerCons(void *arguments);
void* ThreadHandlerProd(void *arguments);
