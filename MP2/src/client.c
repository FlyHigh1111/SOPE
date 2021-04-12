#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024

struct Arguments
{
    size_t nsecs; // number of seconds
    char *public_fifo;
};

int errno;

void *Request(void *arg)
{
    fprintf(stdout, "ola\n");
    pthread_exit(NULL);
}

void ParseArguments(int argc, char *argv[], struct Arguments *args)
{
    if(argc < 4 || strcmp(argv[0], "./c") || strcmp(argv[1], "-t"))
    {
        fprintf(stderr, "Usage: c <-t nsecs> fifoname\n");
        exit(1);
    } 
    if(argv[2] < 0) 
    {
        fprintf(stderr, "Number of seconds should be positive!\n");
        exit(1);
    } 

    args->nsecs = atoi(argv[2]);
    args->public_fifo = argv[3];
}

int main(int argc, char *argv[], char *envp[])
{
    struct Arguments args;
    ParseArguments(argc, argv, &args);
    int fd = mkfifo(args.public_fifo, S_IWUSR | S_IWOTH | S_IWGRP);

    if(fd == -1){
        fprintf(stderr, "Error creating FIFO");
        return 1;
    }

    int open_fd = open(args.public_fifo, O_WRONLY);

    if(open_fd == -1){
        fprintf(stderr, "Error opening FIFO");
        return 1;
    }
    
    size_t i = 1;

    time_t inst;
    inst = time(NULL);

    for (time_t ns = inst; ns < inst + args.nsecs; ns++)
    {
        //falta me fazer o que cada thread envia para o FIFO publico

        pthread_t tid;
        if(pthread_create(&tid, NULL, Request, NULL) != 0)
        {
            fprintf(stderr, "Error: %d\n", errno);
        }
        pthread_join(tid, NULL);
    
        // estrutura: inst ; i ; t ; pid ; tid ; res ; oper
        fprintf(stdout, "inst = %ld ; i = %ld ; t ; pid C = %d ; tid = %ld ; res ; oper = IWANT \n", ns, i, getpid(), tid);

        i++;
    }
     
     close(open_fd);

    return 0;
}
