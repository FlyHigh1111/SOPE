#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

struct Arguments
{
    char **options;
    char *mode;
    char *file_path;
};

/**
 * Initializes Arguments struct with the values introduced in the command line and verifies errors
 * @param argc: number of arguments
 * @param argv: array of arguments
 * @param args: Arguments struct passed by reference
 * @return int: returns 0 if success; 1 otherwise
 */
int InitializeArguments(int argc, char *argv[], struct Arguments *args);
