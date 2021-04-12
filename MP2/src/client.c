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


pthread_mutex_t lock;

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
void* fthread(){
    int fp;
    char nfifopriv[200];
    //cria ffifo privado
    pthread_mutex_lock(&lock);
    sprintf(nfifopriv,"/tmp/%d.%ld",getpid(),pthread_self());
    printf("nfifo: %s \n",nfifopriv);
    mkfifo(nfifopriv, 0666);//cria fifo privado
    pthread_mutex_unlock(&lock);
    //construir mensagem que vai ser enviado servidor:gerar numero 0-9
    //write mensagem no fifo publico (fd fornecido com argumento da funçao)
    fp=open(nfifopriv,O_NONBLOCK,O_WRONLY);//futuro tirar nonblock
    printf("p:%d tid:%ld \n",fp,pthread_self());
    //read resposta do servidor 
    //publicar resultado 
    //fechar fifoprivado 
    //eliminar fifoprivado
    
}

int main(int argc, char *argv[], char *envp[])
{
    struct Arguments args;
    ParseArguments(argc, argv, &args);
    int fd = mkfifo(args.public_fifo, S_IWUSR | S_IWOTH | S_IWGRP);//sair porque este é o fifo publico cujo ja esta criado e nome é passado por parametro, nao é o cliente a criar o fifo

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
    
    /*pthread_t id[20];

    pthread_mutex_init(&lock, NULL);
    for(int i=1;i<=20;i++){
        pthread_create(&id[i], NULL, fthread, NULL);

    }
    for(int i=1;i<=20;i++){
        pthread_join(id[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    */
     
     close(open_fd);

    return 0;
}
