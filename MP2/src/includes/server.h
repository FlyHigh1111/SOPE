#pragma once
#include "./common.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "client.h"

#define BUFF_SIZE 1024;
struct Arguments
{
    size_t nsecs;      //number of seconds
    int buffer_size;    //buffer size
    char *public_fifo; //public FIFO
};
bool finish=false;

bool isNumeric(char num[]);
void sigAlrmHandlerS(int signum);