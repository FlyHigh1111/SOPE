#include "./includes/server.h"
#include "./includes/common.h"
#include "./includes/queue.h"

bool isNumeric(char num[] )
{
    for(int i = 0; i < strlen(num); i++)
    { 
        if(!isdigit(num[i]))
            return false;
    }
    return true;
}

void ParseArguments(int argc, char *argv[], struct Arguments *args)
{
    if(argc !=4 && argc!=6 )
    {
        fprintf(stderr, "Usage: s <-t nsecs> [-l buffsize] fifoname\n");
        exit(1);
    } 

    if( strcmp(argv[1], "-t") || (argc==6 && strcmp(argv[3],"-l")))
    {
        fprintf(stderr, "Usage: s <-t nsecs> [-l buffsize] fifoname\n");
        exit(1);
    } 

    if(!isNumeric(argv[2]) || atoi(argv[2])<0){
        fprintf(stderr, "Number of seconds should be a positive integer!\n");
        exit(1);

    }

    if(argc==6){
        if(!isNumeric(argv[4]) || atoi(argv[4]) < 0)
        {
            fprintf(stderr, "Buffer size should be a positive integer!\n");
            exit(1);
        }
        args->buffer_size=atoi(argv[4]);
        strcpy(args->public_fifo,argv[5]);
    }
    args->nsecs = atoi(argv[2]);

    if(argc==4)
    {
        args->buffer_size = BUFF_SIZE;
        strcpy(args->public_fifo,argv[3]);      
    }
}

void WriteMsg(char *oper, struct Message msg){
    time_t inst = time(NULL);
    // structure: inst ; i ; t ; pid ; tid ; res ; oper
    fprintf(stdout, "%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n", inst, msg.rid, msg.tskload, msg.pid, msg.tid, msg.tskres, oper);
}

void sigAlrmHandlerS(int signum)
{
    finish = true;
}

void* ThreadHandlerCons(void *arguments)
{
    //struct Queue queue;
    struct Message response_message;
    struct ArgsThreadsConsumer* args=(struct ArgsThreadsConsumer*)arguments;
    char private_fifo[BUFFER_SIZE];
    sem_wait(&semFull);

    while(1)
    {
            pthread_mutex_lock(&lock);
            count--;
            response_message = args->cloud[count]; 
            pthread_mutex_unlock(&lock);
        
          //writes response in the private fifo
          snprintf(private_fifo, BUFFER_SIZE, "/tmp/%d.%ld", response_message.pid, response_message.tid);
          int fd_private_fifo = open(private_fifo, O_WRONLY);
          int num = write(fd_private_fifo, &response_message, sizeof(response_message));

            if(num == -1){
                WriteMsg("FAILD", response_message);
            }
            else{
                WriteMsg("TSKDN", response_message);
            }
        sem_post(&semEmpty);
        pthread_exit(NULL);
    }
}


void* ThreadHandlerProd(void *arguments)
{
    struct Message response_message;
    
    struct ArgsThreadsProducer* args = (struct ArgsThreadsProducer*)arguments;
    sem_wait(&semEmpty);

    //builds response to put in the cloud
    response_message.rid=args->rid;
    response_message.pid=args->pid;
    response_message.tskload=args->tskload;
    response_message.tid=args->tid;
    
    //calls function from library (lib) to obtain the task load of the request
    response_message.tskres=task(args->tskload);
    //printf("produto entrada ciclo: %d \n", queueIsFull(&queue,args->nmax));

    if(response_message.tskres){
        WriteMsg("TSKEX", response_message);
    }
    
    pthread_mutex_lock(&lock);
    count++;
    args->cloud[count] = response_message;
    pthread_mutex_unlock(&lock);
    sem_post(&semFull);

    pthread_exit(NULL);
}

int main(int argc,char** argv)
{
    struct Arguments args;
   
/*if (argc==1) {
    args.buffer_size=10;
    strcpy(args.public_fifo,"testefifo");
    args.nsecs = 10;
}*/  
    struct ArgsThreadsProducer argsthsprod[MAX_THREADS];
    struct ArgsThreadsConsumer argsthscon;
    struct Message request_message;

    ParseArguments(argc,argv,&args);

    pthread_t tid[MAX_THREADS] = {0};
    sem_init(&semEmpty, 0, args.buffer_size);
    sem_init(&semFull, 0, 0);

    //creates public fifo 
    mkfifo(args.public_fifo,0666);
    signal(SIGALRM,sigAlrmHandlerS);

    //opens public fifo for reading
    int fd_publicfifo = open(args.public_fifo,O_NONBLOCK,O_RDONLY);
    if(fd_publicfifo == -1)
    {
        fprintf(stderr, "Error: %d\n", errno);
        return 0;
    }

    //allocates space in the heap for the cloud
    struct Message *cloud = (struct Message*)malloc(sizeof(struct Message)*args.buffer_size);//paramentro de buff_size em nº
    
    //initQueue(&queue);
    
    //initializes args to use in handler of consumer thread
    argsthscon.cloud = cloud;
    argsthscon.nmax=args.buffer_size;

    //creates consumer thread
    pthread_create(&tid[0],NULL,&ThreadHandlerCons,&argsthscon); 
    int th=1;//thread counter

    alarm(args.nsecs);
    int j=0;
    while(!finish)
    {
        //reads requests coming from the public fifo
        if((j = read(fd_publicfifo, &request_message, sizeof(struct Message))) > 0)
        {   printf("th: %d",th);
            argsthsprod[th].rid=request_message.rid;
            argsthsprod[th].tid=request_message.tid;
            argsthsprod[th].pid =request_message.pid;
            argsthsprod[th].tskres=request_message.tskres;
            argsthsprod[th].tskload=request_message.tskload;
            argsthsprod[th].cloud=cloud;
            argsthsprod[th].nmax=args.buffer_size;
            pthread_create(&tid[th],NULL,&ThreadHandlerProd,&argsthsprod[th]);
            th++;
       

        }     
    }

    //main thread waits for the producer (k>=1) and consumer (k=0) threads to finish 
    for(int k = th; k >= 0; k--)
    {
        pthread_join(tid[k], NULL);
    }

    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    close(fd_publicfifo);
    unlink(args.public_fifo);
    free(cloud);

    return 0;

}
