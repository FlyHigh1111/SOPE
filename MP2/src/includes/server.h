#pragma once
#include "./common.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


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

bool finish=false;

bool isNumeric(char num[]);
void sigAlrmHandlerS(int signum);