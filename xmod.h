#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


struct Arguments
{
    char **options;
    char *mode;
    char *file_path;
};

/**
 * Initializes Arguments struct with the values introduced in the command line.
 * @param argc: number of arguments
 * @param argv: array of arguments
 * @return struct Arguments: Arguments struct wtih data members initialized
 */
struct Arguments InitializeArguments(int argc, char *argv[]);
