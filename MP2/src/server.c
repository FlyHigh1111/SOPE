#include "./includes/server.h"

bool isNumeric(char num[] ){
    for(int i=0;i<strlen(num);i++)
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
        if(!isNumeric(argv[4])|| atoi(argv[4])<0){
            fprintf(stderr, "Buffer size should be a positive integer!\n");
            exit(1);
        }
        args->buffer_size=atoi(argv[4]);
        args->public_fifo = argv[5];
    }
    args->nsecs = atoi(argv[2]);

    if(argc==4){
        args->buffer_size=BUFF_SIZE;
        args->public_fifo = argv[3];
    }
}
void sigAlrmHandlerS(int signum){
    finish=true;
}

void* ThreadHandlerCons(){

}
void* ThreadHandlerProd(){

}

int main(int argc,char** argv){
    struct Arguments args;
    struct Queue queue;
    struct ArgsThreadSProd argsthsprod;
    struct ArgsThreadSCon argsthscon;
    struct Message request_message;
    ParseArguments(argc,argv,&args);
   pthread_t tid[MAX_THREADS] = {0};

    //creates public fifo 
    mkfifo(args.public_fifo,0666);
    signal(SIGALRM,sigAlrmHandlerS);
    //opens public fifo for reading
    int fd_publicfifo=open(args.public_fifo,O_NONBLOCK,O_RDONLY);

    //aloca espaço na heap  para armazem
    struct Message *armazem=(struct Message*)malloc(sizeof(struct Message)*args.buffer_size);//paramentro de buff_size em nº
    //struct Message *armazem=(struct Message*)malloc(args.buffer_size);//paramentro de buff_size em bytes
    //inicialiiza cabeça da fila do armazem
    queue.primeiro=-1;
    queue.ultimo=-1;

    //cria thread consumidor
    pthread_create(&tid[0],NULL,ThreadHandlerCons,&argsthscon); 


    alarm(args.nsecs);
    
    while(!finish){
        //ler os pedidos que chegam pelo fifopublico
        if(read(fd_publicfifo, &request_message, sizeof(struct Message))!=-1){
            argsthsprod.rid=request_message.rid;
            argsthsprod.tid=request_message.tid;
            argsthsprod.pid =request_message.pid;
            argsthsprod.tskres=request_message.tskres;
            argsthsprod.tskload=request_message.tskload;
            argsthsprod.queue=queue;
            pthread_create(&tid[0],NULL,ThreadHandlerProd,&argsthsprod);
            printf("Processa pedidos\n");
        }


        
    }
    //liberta espaço do buffer (armazem)
    free(armazem);

    return 0;

}




