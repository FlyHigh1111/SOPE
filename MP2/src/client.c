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
#define CLIENTRES -1


pthread_mutex_t lock;

struct Arguments
{
    size_t nsecs; // number of seconds
    char *public_fifo;
};

struct Request {
    int fd; //public FIFO identifier
    int i;
    int t;
    pid_t pid;
    pthread_t tid;
    int res;
    char *oper;
};

int errno;

void *Test(void *arg)
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
void* fthread(void *param){
    int fp;
    char nfifopriv[200];
    struct Request *aux = (struct Request *) param;
    aux->tid = pthread_self();

    //cria ffifo privado
    pthread_mutex_lock(&lock);
    sprintf(nfifopriv,"/tmp/%d.%ld",getpid(),pthread_self());
    printf("nfifo: %s \n",nfifopriv);
    mkfifo(nfifopriv, 0666);//cria fifo privado
    pthread_mutex_unlock(&lock);
    //construir mensagem que vai ser enviado servidor:gerar numero 0-9
    //write mensagem no fifo publico (fd fornecido com argumento da funçao)
    int fd = write(aux->fd, "%d ; %d ; %d ; %d ; %d ; %s", sizeof(aux));

    if(fd == -1){
        fprintf(stderr, "Error writing to public FIFO");
    }
    
    fp=open(nfifopriv,O_NONBLOCK,O_WRONLY);//futuro tirar nonblock
    printf("p:%d tid:%ld \n",fp,pthread_self());
    //read resposta do servidor 
    //publicar resultado 
    //fechar fifoprivado 
    //eliminar fifoprivado
    return NULL;
}


void WriteLog(struct Request request){
    fprintf(stdout, "%d ; %d ; %d ; %ld ; %d ; %s", request.i, request.t, request.pid, request.tid, request.res, request.oper);
}


int RandomInt(){
    return rand() % 10;
}


int main(int argc, char *argv[], char *envp[])
{
    struct Arguments args;
    struct Request request;
    srand(time(NULL));


    ParseArguments(argc ,argv, &args);

    //open public FIFO
    request.fd = open(args.public_fifo, O_WRONLY);
    if(request.fd == -1){
        fprintf(stderr, "Error opening FIFO");
        return 1;
    }
    
    size_t i = 1;
    time_t inst;
    inst = time(NULL);

    for (time_t ns = inst; ns < inst + args.nsecs; ns++)
    {

        pthread_t tid;
        request.i = i;
        request.t = RandomInt();
        request.pid = getpid();
        request.res = CLIENTRES;
        request.oper = "Teste";
        if(pthread_create(&tid, NULL, fthread, &request) != 0)
        {
            fprintf(stderr, "Error: %d\n", errno);
        }
        request.tid = tid;
        pthread_join(tid, NULL);
    
        // estrutura: inst ; i ; t ; pid ; tid ; res ; oper
        WriteLog(request);
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
     
     close(request.fd);

    return 0;
}
