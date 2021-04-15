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
struct argsThread{
    pid_t pid;
    int fPubl;
};
struct message {
	int rid;	// request id
	pid_t pid;	// process id
	pthread_t tid;	// thread id
	int tskload;	// task load
	int tskres;	// task result
	};//struct para troca de mensagens entre cliente e servidor ,conforme slack 10 abr


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
/*void* fthread(void *param){
    int fp;
    char nfifopriv[200];
    struct Request *aux = (struct Request *) param;
    aux->tid = pthread_self();

    //cria ffifo privado
    //pthread_mutex_lock(&lock);
    sprintf(nfifopriv,"/tmp/%d.%ld",getpid(),pthread_self());
    printf("nfifo: %s \n",nfifopriv);
    mkfifo(nfifopriv, 0666);//cria fifo privado
    //pthread_mutex_unlock(&lock);
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
}*/
#define MAX_THREADS 10000

int cont;              //contador que regista o numero de threads criados pelo thread principal.
int terminar=0;
pthread_mutex_t lock;  //variavel que contem o estado de um mutex (aberto ou fechado)

//Função para gerar um numero aleatorio no intervalo [inf,sup]:
int aleatorio(int inf, int sup);
//Handler para o sinal SIGPIPE
void sigpipeHandler();



void* thread_handle(void *argumentos){
    int i;                  //numero universal unico de pedido
    int t;                  //carga de um pedido
    char nFifoPriv[256];    //nome (em sistema) do fifo privado
    int fpriv;                 //descritor fifo privado
    //char mensPedido[256];   //mensagem que contem um pedido 
    //char mensResposta[256]; //mensagem que contem uma resposta
    struct message mensPedido;//mensagem que contem um pedido 
    struct message mensResposta;///mensagem que contem uma resposta

    struct argsThread *args=(struct argsThread*) argumentos;

  //gera numero universal unico de pedido:
    pthread_mutex_lock(&lock);
    cont++;
    i=cont;
    pthread_mutex_unlock(&lock);  

  //cria fifo privado
    sprintf(nFifoPriv,"/tmp/%d.%ld",getpid(),pthread_self());
    mkfifo(nFifoPriv, 0666);

  //gera a carga do pedido (numero aleatorio entre 1 e 9):
    t=aleatorio(1,9);

  //constrói a mensagem que vai ser enviada ao servidor:
    //sprintf(mensPedido,"%d %d %d %ld -1",i,t,getpid(),pthread_self());
    mensPedido.rid=i;
    mensPedido.tskload=t;
    mensPedido.tskres=CLIENTRES;
    mensPedido.pid=args->pid;
    mensPedido.tid=pthread_self();

  //abre o fifo publico para escrita (fPublico fornecido como argumento da funçao?)
    pthread_mutex_lock(&lock);
    write(args->fPubl,&mensPedido,sizeof(struct message));
    pthread_mutex_unlock(&lock);

    //abre o fifo privado para leitura:  
    fpriv=open(nFifoPriv,O_RDONLY);
    //lê a resposta do servidor bloqueia  enquanto a resposta nao chegar   
    read(fpriv,&mensResposta,sizeof(struct message));

    //IMPRIME RESULTADO? OU PASSA-O PARA O THREAD PRINCIPAL? FALTA

    //prepara a saida:
    close(fpriv);
    //elimina fifo privado:
    unlink(nFifoPriv);  
    
    //termina o thread:
    pthread_exit(NULL);   
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
    struct argsThread argsth;

    srand(time(NULL));
    cont=0;//global variavel updated by each thread

    ParseArguments(argc ,argv, &args);

    //open public FIFO
    int fd = open(args.public_fifo, O_WRONLY);
    if(fd == -1){
        fprintf(stderr, "Error opening FIFO");
        return 1;
    }
    
    size_t i = 1;
    time_t inst;
    inst = time(NULL);

    //inicializa argumentos a passar para a funçao executada por cada thread
    argsth.pid=getpid();
    argsth.fPubl=fd;
    int th=0;
    pthread_t tid[MAX_THREADS];
  
    for (time_t ns = inst; ns < inst + args.nsecs; ns++)
    {
      
        /*request.i = i;
        request.t = RandomInt();
        request.pid = getpid();
        request.res = CLIENTRES;
        request.oper = "Teste";*/
        
        /*if(pthread_create(&tid, NULL, thread_handle, &request) != 0)
        {
            fprintf(stderr, "Error: %d\n", errno);
        }
        request.tid = tid;
        pthread_join(tid, NULL);
    */
        if(pthread_create(&tid[th], NULL, thread_handle, &argsth) != 0){
            fprintf(stderr, "Error: %d\n", errno);
        }
        
        // estrutura: inst ; i ; t ; pid ; tid ; res ; oper
        WriteLog(request);
        fprintf(stdout, "inst = %ld ; i = %ld ; t ; pid C = %d ; tid = %ld ; res ; oper = IWANT \n", ns, i, getpid(), tid);

        th++;
    }
    for(int k=0;k<th;k++){
        pthread_join(tid[k],NULL);
    }
     
     close(request.fd);

    return 0;
}
