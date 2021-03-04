#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "xmod.h"


int decimalToOctal(int decimalnum)
{
    int octalnum = 0, temp = 1;
    
    while (decimalnum != 0)
    {
        octalnum = octalnum + (decimalnum % 8) * temp;
        decimalnum = decimalnum / 8;
        temp = temp * 10;
    }
    
    return octalnum;
}

char *options;
char *mode;
char *pathname;

int main(int argc, char *argv[]) {
    
    if (argc < 3 || argc > 6 ){
        printf("ERRO - Numero Invalido de Parametros!\n");
        return (1);
    }
    int i;
    //Inicializa Parâmetros:
    if (argc>3){   //verifica se foram definidas uma ou mais opções
        options=malloc((argc-3)*sizeof(char));
        for (i=1;i<=argc-3;i++){
            if (argv[i][0]!='-'){
                printf("Erro - opçao invalida!\n");
                return 1;
            }
            else if (argv[i][1]!='v' && argv[i][1]!='c' && argv[i][1]!='R'){
                printf("Erro - opçao invalida!\n");
                return 1;
            }
            else if (strlen(argv[i])!=2){
                printf("Erro - opçao invalida!\n");
                return 1;
            }
            options[i-1]=argv[i][1];
        }
        options[i]='\0';
    }
    else
        options=NULL;
    
    //Inicializa mode a partir da lista de argumentos da lnha de comando:
    mode=malloc(sizeof(argv[argc-2]));
    mode=argv[argc-2];
    //Inicializa pathname a partir da lista de argumentos da lnha de comando:
    pathname=malloc(sizeof(argv[argc-1]));
    pathname=argv[argc-1];
    
    //Processa Opcoes
    
    //Processa Modo:
    //Verificar formato fornecido:
    if (mode[0]=='0'){ //modo indicado em formato "octal"
        if (strlen(mode)!=4){
            printf("Erro - modo invalido!\n");
            return 1;
        }
        for (int i=1;i<4;i++){
            if ((mode[i]<'0')||(mode[i]>'7')){ //verifica se modo é um numero em octal
                printf("Erro - modo invalido!\n");
                return 1;
            }
        }
        //int strtoul(const char *str, char **endptr, int base)
        printf("modo formato octal, correcto\n");
        //printf("teste modo: %d   %o",atoi(mode),decimalToOctal(atoi(mode)));
        if(chmod(pathname,strtoul(mode,NULL,8))!=0)
            printf("erro no chmod\n");
        
    }
    else{
        char dominio[8];
        strcpy(dominio,"augo-+=");
        if(strchr(dominio,mode[0])==NULL){
            printf("Erro - modo invalido!\n");
            return 1;
        }
        printf("modo formato rwx, correcto\n");
        //modo em formato rwx
        
    }
    //Executa o comando:
    
    
    
    
    
    
    
    //printf ("opcoes: %s\nmode: %s\npathname: %s\n",options,mode,pathname);
    
    
    
    return 0;
}









/*
#include "xmod.h"

struct Arguments InitializeArguments(int argc, char *argv[])
{
    struct Arguments args;

    //verificar se argc está mal e print usage (implementar quando tivermos a certeza de quantos argumentos são ao certo).
    printf("Usage: xmod [OPTIONS] MODE FILE/DIR\n");
    printf("Usage: xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");

    //counts the number of options introduced in the command line
    int num_options = 0;
    for(size_t k = 0; k < argc; k++)
    {
        if(argv[k][0] == '-')
        {
            num_options++;
        }
    }
    printf("num_options: %d\n", num_options);

    if(num_options > 0)
    {
        //allocating memory for array of options
        args.options = malloc(num_options * sizeof(char *)); 
        if (args.options != NULL)
        {
            for (int i = 0 ; i < num_options ; i++)
            {
                int number_letters = 2;
                args.options[i] = malloc(number_letters); //each option has only 2 characters (e.g. '-v')
                                                          //this may change
            }
        }  
        //initializes the array of options
        for(size_t j = 0; j < num_options; j++)
        {
            strcpy(args.options[j], argv[j+1]);
        }

    } 

    //prints options (just to test if it is ok)
    for (size_t k = 0; k < num_options; k++)
    {
        printf("%s\n", args.options[k]);
    }

    args.mode = argv[num_options+1];
    args.file_path = argv[num_options+2];

    printf("mode: %s\n", args.mode);
    printf("file_path: %s\n", args.file_path);

    return args;
}

int main( int argc, char *argv[])  
{
    struct Arguments args;

    args = InitializeArguments(argc, argv);

	return 0;
}
*/
