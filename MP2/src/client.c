#include "client.h"
#include <pthread.h>


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

int Randomize(int lower, int upper, unsigned int seed)
{
    return (rand_r(&seed) % (upper - lower + 1)) + lower;
   
}


void* ThreadHandler(void *arguments)
{
    int i;                  
    int t; 
    int j;                 
    char private_fifo[BUFFER_SIZE];    //private FIFO name
    int fd_private_fifo;   //private FIFO file descriptor
   
    struct Message request_message; //message with the request of the client
    struct Message response_message;  //message with the response from teh server

    struct ArgsThread *args= (struct ArgsThread*)arguments;

    pthread_mutex_lock(&lock1);
    j=cont;
    cont++;
    i = cont;
    pthread_mutex_unlock(&lock1);  

    //creates private fifo
    //sprintf(private_fifo, "/tmp/%d.%ld", getpid(), pthread_self());
    snprintf(private_fifo,BUFFER_SIZE, "/tmp/%d.%ld", getpid(), pthread_self());
    mkfifo(private_fifo, 0666);

    unsigned int seed = (unsigned) i;

    //generates task load (random number between 1 and 9):
    t = Randomize(1, 9, seed);

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

    pthread_mutex_lock(&lock2);
    write(args->fd_public_fifo,&request_message,sizeof(struct Message));
    pthread_mutex_unlock(&lock2);

    //opens private FIFO por reading
    fd_private_fifo = open(private_fifo, O_RDONLY);
    fdt[j]=fd_private_fifo;
    
    //reads server response and blocks while the server does not respond 
    if(read(fd_private_fifo, &response_message, sizeof(struct Message))==-1){
    
        if(!termina){
            log.oper="GAVUP";
            log.res=-1;
        }
    }
   else{
    //checks server response (get last param in order to check if service  is closed)
    if(response_message.tskres==-1){
        log.oper="CLOSD";
        termina=false;
    }
    else{
        log.res=response_message.tskres;
        log.oper = "GOTRS";
    }
   }
    /*if(!termina){
    
    log.oper="GAVUP";
    }*/
    
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

void sigAlrmHandler(){
    termina=false;

}


int main(int argc, char *argv[], char *envp[])
{
    struct Arguments args;
    struct ArgsThread argsth;

   signal(SIGALRM,sigAlrmHandler);


    cont = 0; //variable updated by each thread

    ParseArguments(argc ,argv, &args);
    int fd = 0;
    //open public FIFO
    while(1){
        fd = open(args.public_fifo, O_WRONLY);
        if(fd != -1)
        {
            //fprintf(stderr, "Error opening FIFO");
            break;
        }
    }
    //initializes thread arguments to use in thread_handle function
    argsth.pid = getpid();
    argsth.fd_public_fifo = fd;

    int th = 0; //thread counter
    pthread_t tid[MAX_THREADS] = {0}; //array with thread's tids
  
    //time_t inst;
    //inst = time(NULL);
    //time_t ns = inst;
    //invoca funçao alarm para despoletar o SIGALRM ao fim nsecs
    alarm(args.nsecs);
    while(termina)
     //while(ns < inst + args.nsecs)
    {
        if(pthread_create(&tid[th], NULL, ThreadHandler, &argsth) != 0){
            fprintf(stderr, "Error: %d\n", errno);
        }
        usleep(5000); //5ms
        //ns = time(NULL);
        th++;
    }
usleep(3000);
/*char str[256];
for(int i=0;i<th;i++){
    snprintf(str,256,"/tmp/%d.%ld",getpid(),tid[i]);
    remove(str);
   
    
}*/
for(int i=0;i<cont;i++){
    close(fdt[i]);
    printf("fdt: %d \n",fdt[i]);
}

    for(int k = 0; k < th; k++)
    {
        pthread_join(tid[k], NULL);
    }
     
    close(argsth.fd_public_fifo);

    return 0;
}