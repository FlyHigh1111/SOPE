#include "./includes/server.h"
#include "./includes/queue.h"

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

void* ThreadHandlerCons(void *arguments){
    //struct Queue queue;
    struct Message response_message;
    struct ArgsThreadSCon* args=(struct ArgsThreadSCon*)arguments;
    char private_fifo[BUFFER_SIZE];

    while(1){
        //verificar se fila vazia
        if(!queueIsEmpty(&queue)){
            //aceder ao armazem e retirar a prox mensagem
            topQueue(&queue,args->armazem,&response_message);
            popQueue(&queue,args->armazem,args->nmax);
             //escrever resposta no fifoprivado correspondente do cliente
            snprintf(private_fifo,BUFFER_SIZE, "/tmp/%d.%ld",response_message.pid,response_message.tid);
            int fd_private_fifo = open(private_fifo, O_RDONLY);
            write(fd_private_fifo,&response_message,sizeof(response_message));

        }
        
            //verificar se fila vazia
            
        
        
    }
}
void* ThreadHandlerProd(void *arguments){
    
    struct Message response_message;
    
    struct ArgsThreadSProd* args=(struct ArgsThreadSProd*)arguments;
    printf("tskload_entrada: %d tid :%ld tid_self:%ld \n",args->tskload,args->tid,pthread_self());
    //constroi resposta a colocar no armazem
    response_message.rid=args->rid;
    response_message.pid=args->pid;
    response_message.tskload=args->tskload;
    response_message.tid=args->tid;
    //printf("tskload:%d /n",args->tskload);
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
   
    struct ArgsThreadSProd argsthsprod[MAX_THREADS];
    struct ArgsThreadSCon argsthscon;
    struct Message request_message;
    ParseArguments(argc,argv,&args);
   pthread_t tid[MAX_THREADS] = {0};

    //creates public fifo 
    mkfifo(args.public_fifo,0666);
    signal(SIGALRM,sigAlrmHandlerS);
    //opens public fifo for reading
    int fd_publicfifo=open(args.public_fifo,O_NONBLOCK,O_RDONLY);
    if(fd_publicfifo==-1){
        perror("Erro na abertura do fifo ");
        return 0;
    }

    //aloca espaço na heap  para armazem
    struct Message *armazem=(struct Message*)malloc(sizeof(struct Message)*args.buffer_size);//paramentro de buff_size em nº
    
    //inicialiiza cabeça da fila do armazem
    initqueue(&queue);
    
    //inicializa args para handler do thread consumidor
    argsthscon.armazem=armazem;
    argsthscon.nmax=args.buffer_size;

    //cria thread consumidor
    pthread_create(&tid[0],NULL,&ThreadHandlerCons,&argsthscon); 
    int th=1;//inicializa thread counter

    //inicia contagem de tempo para emissao sinal sigalrm
    alarm(args.nsecs);
    int j;
    while(th<=10){
        //ler os pedidos que chegam pelo fifopublico
        if((j=read(fd_publicfifo, &request_message, sizeof(struct Message)))>0){
            argsthsprod[th].rid=request_message.rid;
            argsthsprod[th].tid=request_message.tid;
            argsthsprod[th].pid =request_message.pid;
            argsthsprod[th].tskres=request_message.tskres;
            argsthsprod[th].tskload=request_message.tskload;
            argsthsprod[th].armazem=armazem;
            argsthsprod[th].nmax=args.buffer_size;
            printf("pid %d tid %ld tskload %d  th: %d\n",request_message.pid,request_message.tid,request_message.tskload,th);
            pthread_create(&tid[th],NULL,&ThreadHandlerProd,&argsthsprod[th]);
            th++;
       j=0;

        }     
    }
    printf("saiu \n");
    //thread principal espera que threads produtores (k>=1) e consumidor (k=0) terminem
    for(int k = 0; k < th; k++)
    {
        pthread_join(tid[k], NULL);
    }

    //fecha fifpublico depois de terminado o tempo de execuçao do servidor 
    close(fd_publicfifo);
    unlink(args.public_fifo);
    //liberta espaço do buffer (armazem)
    free(armazem);

    return 0;

}




