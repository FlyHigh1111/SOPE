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
    if(!isNumeric(argv[2]) || atoi(argv[2]<0)){
        fprintf(stderr, "Number of seconds should be a positive integer!\n");
        exit(1);

    }

    if((!isNumeric(argv[4]) || atoi(argv[4]<0)) && argc==6){
        fprintf(stderr, "Buffer size should be a positive integer!\n");
        exit(1);
    }


    args->nsecs = atoi(argv[2]);

    if(argc==4){
    
        args->public_fifo = argv[3];
    }
    if(argc==6){
        args->buffer_size=atoi(argv[4]);
        args->public_fifo = argv[5];
    }
}


int main(int argc,char** argv){
    struct Arguments args;
    ParseArguments(argc,argv,&args);
    printf("nsec: %ld buff: %d fifoname: %s", args.nsecs,args.buffer_size,args.public_fifo);
    return 0;

}




