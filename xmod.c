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
