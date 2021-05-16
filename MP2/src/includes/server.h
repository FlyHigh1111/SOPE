#pragma once

#include "./common.h"

#define MAX_THREADS 10000
#define SERVER -1

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

bool finish=false;

bool isNumeric(char num[]);
void sigAlrmHandlers(int signum);
void WriteLog(struct Log log);