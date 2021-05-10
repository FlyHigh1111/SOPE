#pragma once
#include "./common.h"
#include <stdio.h>
#include <ctype.h>

#define BUFF_SIZE 1024;
struct Arguments
{
    size_t nsecs;      //number of seconds
    int buffer_size;    //buffer size
    char *public_fifo; //public FIFO
};

bool isNumeric(char num[]);
