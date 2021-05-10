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


int main(int argc,char** argv){
    struct Arguments args;
    struct ArgsThread argsth;
    ParseArguments(argc,argv,&args);
    //creates public fifo 
    mkfifo(args.public_fifo,0666);
    signal(SIGALRM,sigAlrmHandlerS);

    
    //opens public fifo for reading
    int fd_publicfifo=open(args.public_fifo,O_RDONLY);

    //Implementar armazem com fila (cabeça da fila com ponteiros para inicio e fim ) 
    //funçao push->mete elemento no final da fila
    //funçao pop()->retira elemento da fila no inicio
    alarm(args.nsecs);
    while(!finish){
        printf("Processa pedidos");
    }



    return 0;

}




