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

void ThreadHandlerCons(void *arguments){
    //struct Queue queue;
    struct Message response_message;
    struct ArgsThreadSCon* args=(struct ArgsThreadSCon*)arguments;
    char private_fifo[BUFFER_SIZE];

    while(1){
        //aceder ao armazem e retirar a prox mensagem
            //verificar se fila vazia
            //top do valor do inicio da fila 
            //pop 
        //escrever resposta no fifoprivado correspondente do cliente
        snprintf(private_fifo,BUFFER_SIZE, "/tmp/%d.%ld",response_message.pid,response_message.tid);
        int fd_private_fifo = open(private_fifo, O_RDONLY);
        write(fd_private_fifo,&response_message,sizeof(response_message));
        
        
    }
}
void ThreadHandlerProd(void *arguments){
    
    struct Message response_message;
    
    struct ArgsThreadSProd* args=(struct ArgsThreadSProd*)arguments;

    //constroi resposta a colocar no armazem
    response_message.rid=args->rid;
    response_message.pid=args->pid;
    response_message.tskload=args->tskload;
    response_message.tid=args->tid;

    //chama biblioteca para obter resultado em funçao da carga(tskload) do pedido
    response_message.tskres=task(args->tskload);

    
    //colocar resposta no armazem
    pthread_mutex_lock(&lock);
    while(pushbackqueue(&queue,args->armazem, response_message,args->nmax)){

    }
    pthread_mutex_unlock(&lock);

    


    
    pthread_exit(NULL);//termina thread
}

int main(int argc,char** argv){
    struct Arguments args;
   
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
    
    //inicialiiza cabeça da fila do armazem
    queue=initqueue();
    

    //cria thread consumidor
    pthread_create(&tid[0],NULL,ThreadHandlerCons,&argsthscon); 

    //inicia contagem de tempo para emissao sinal sigalrm
    alarm(args.nsecs);
    
    while(!finish){
        //ler os pedidos que chegam pelo fifopublico
        if(read(fd_publicfifo, &request_message, sizeof(struct Message))!=-1){
            argsthsprod.rid=request_message.rid;
            argsthsprod.tid=request_message.tid;
            argsthsprod.pid =request_message.pid;
            argsthsprod.tskres=request_message.tskres;
            argsthsprod.tskload=request_message.tskload;
            argsthsprod.armazem=armazem;
            argsthsprod.nmax=args.buffer_size;
           
            pthread_create(&tid[0],NULL,ThreadHandlerProd,&argsthsprod);
            printf("Processa pedidos\n");
        }


        
    }
    //liberta espaço do buffer (armazem)
    free(armazem);

    return 0;

}




