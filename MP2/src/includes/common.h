#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include "lib.h"

#define BUFF_SIZE 1024;
#define BUFFER_SIZE 256
/**
 * @brief Struct for exchange of messages between client and server
 */
struct Message{
	int rid; 		// request id
	pid_t pid; 	    // process id
	pthread_t tid;	// thread id
	int tskload;    // task load
	int tskres;		// task result
};

/**
 * @brief Struct with information to print to stdout
 */
struct Log 
{
    time_t inst;   //return value of time() function
    int i;         //unique request number
    int t;         //task load
    pid_t pid;     //process ID
    pthread_t tid; //thread ID
    int res;       //task result
    //char oper[6];    //operation made
    char *oper;
};

void WriteLog(struct Log log);
