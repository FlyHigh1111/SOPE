#include <stdio.h>
#include <time.h>

void PrintOperation(char* oper)
{
    // estrutura: inst ; i ; t ; pid ; tid ; res ; oper
    // sempre que ocorrer um oper chamar a função PrintOperation
    time_t seconds;
    seconds = time(NULL);
    fprintf(stdout, "%ld ; i ; t ; pid ; tid ; res ; %s \n", seconds, oper);

}
int main(int argc, char *argv[], char *envp[])
{
    fprintf(stdout, "Usage: c <-t nsecs> fifoname\n");

    int nsecs = atoi(argv[2]);
    char* fifoname = argv[3];

    PrintOperation("teste");
 
    return 0;
}
