#include "xmod.h"

int InitializeArguments(int argc, char *argv[], struct Arguments *args)
{

    if (argc < 3 || argc > 6 )
    {
        printf("Usage: xmod [OPTIONS] MODE FILE/DIR\n");
        printf("Usage: xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return 1;
    }

    //counts the number of options introduced in the command line
    int num_options = 0;
    int max_options = 3;
    for(size_t k = 1; k <= max_options; k++)
    {
        if(strcmp(argv[k],"-v") == 0 || strcmp(argv[k],"-c") == 0 || strcmp(argv[k],"-R") == 0)
        {
            num_options++;
        }
    }

    if(num_options > 0)
    {
        //allocating memory for array of options
        args->options = malloc(num_options * sizeof(char *)); 
        if (args->options != NULL)
        {
            for (int i = 0 ; i < num_options ; i++)
            {
                int number_chars = 2;
                args->options[i] = malloc(number_chars); //each option has only 2 characters (e.g. '-v')
            }
        }  
        //initializes the array of options
        for(size_t j = 0; j < num_options; j++)
        {
            if(strcmp(argv[j+1], "-v") != 0 && strcmp(argv[j+1], "-c") != 0 && strcmp(argv[j+1],"-R") != 0)
            {
                printf("Available options:\n-v\n-c\n-R\n");
                return 1;
            }
            strcpy(args->options[j], argv[j+1]);
        }

    } 

    //verifies mode
    char *mode = argv[num_options+1];
    if (mode[0] == '0') //mode in octal
    { 
        if (strlen(mode) != 4)
        {
            printf("Invalid mode!\n");
            return 1;
        }
        for (int i = 1; i < 4; i++)
        {
            if ((mode[i] < '0') || (mode[i] > '7'))
            {
                printf("Invalid mode!\n");
                return 1;
            }
        }
    }
    else
    {
        char dominio[8];
        strcpy(dominio,"augo-+=");
        if(strchr(dominio, mode[0]) == NULL)
        {
            printf("Invalid mode!\n");
            return 1;
        }
        
    }
    args->mode = mode;

    //verifies if file exists
    if(fopen(argv[num_options+2], "r") == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    args->file_path = argv[num_options+2];

    //prints arguments
    printf("Options:\n");
    for (size_t k = 0; k < num_options; k++)
    {
        printf("%s\n", args->options[k]);
    }
    printf("mode: %s\n", args->mode);
    printf("file_path: %s\n", args->file_path);

    return 0;
}

int main( int argc, char *argv[])  
{
    struct Arguments args;

    if(InitializeArguments(argc, argv, &args) == 1)
    {
        printf("Something went wrong! Closing...\n");
        return 1;
    }

	return 0;
}

