#include "client.h"


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

int Randomize(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

//Handler para o sinal SIGPIPE
void sigpipeHandler();

void* ThreadHandler(void *arguments)
{
    int i;                  
    int t;                  
    char private_fifo[BUFFER_SIZE];    //private FIFO name
    int fd_private_fifo;   //private FIFO file descriptor
   
    struct Message request_message; //message with the request of the client
    struct Message response_message;  //message with the response from teh server

    struct ArgsThread *args= (struct ArgsThread*)arguments;

    pthread_mutex_lock(&lock);
    cont++;
    i = cont;
    pthread_mutex_unlock(&lock);  

    //cria fifo privado
    sprintf(private_fifo, "/tmp/%d.%ld", getpid(), pthread_self());
    mkfifo(private_fifo, 0666);

    //generates task load (random number between 1 and 9):
    t = Randomize(1, 9);

    //constructs the message/log to print to stdout
    struct Log log;
    log.i = i;
    log.t = t;
    log.pid = args->pid;
    log.tid = pthread_self();
    log.res = CLIENTE;

    log.oper = "IWANT";
    WriteLog(log);

    //constructs the message to send to the server
    request_message.rid = i;
    request_message.tskload = t;
    request_message.tskres = CLIENTE;
    request_message.pid = args->pid;
    request_message.tid = pthread_self();

    pthread_mutex_lock(&lock);
    write(args->fd_public_fifo,&request_message,sizeof(struct Message));
    pthread_mutex_unlock(&lock);

    //opens private FIFO por reading
    fd_private_fifo = open(private_fifo, O_RDONLY);

    //reads server response and blocks while the server does not respond 
    read(fd_private_fifo, &response_message, sizeof(struct Message));
    log.oper = "GOTRS";
    WriteLog(log);


    close(fd_private_fifo);
    unlink(private_fifo);  
    
    pthread_exit(NULL);   
}

void WriteLog(struct Log log)
{
    // estrutura: inst ; i ; t ; pid ; tid ; res ; oper
    fprintf(stdout, "%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n", time(NULL), log.i, log.t, log.pid, log.tid, log.res, log.oper);
}


int main(int argc, char *argv[], char *envp[])
{
    struct Arguments args;
    struct ArgsThread argsth;

    srand(time(NULL));
    cont = 0; //variable updated by each thread

    ParseArguments(argc ,argv, &args);

    //open public FIFO
    int fd = open(args.public_fifo, O_WRONLY);
    if(fd == -1)
    {
        fprintf(stderr, "Error opening FIFO");
        return 1;
    }

    //initializes thread arguments to use in thread_handle function
    argsth.pid = getpid();
    argsth.fd_public_fifo = fd;

    int th = 0; //thread counter
    pthread_t tid[MAX_THREADS]; //array with thread's tids
  
    time_t inst;
    inst = time(NULL);
    time_t ns = inst;
    while(ns < inst + args.nsecs)
    {
        if(pthread_create(&tid[th], NULL, ThreadHandler, &argsth) != 0){
            fprintf(stderr, "Error: %d\n", errno);
        }
        usleep(5000); //5ms
        ns = time(NULL);
        th++;
    }
    for(int k = 0; k < th; k++)
    {
        pthread_join(tid[k], NULL);
    }
     
     close(argsth.fd_public_fifo);

    return 0;
}
