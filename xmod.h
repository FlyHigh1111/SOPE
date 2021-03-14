#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include<sys/times.h>
#include<unistd.h>

struct Arguments
{
    char **options;
    char *mode;
    char *file_path;
    bool mode_is_octal;
};

/**
 * Initializes Arguments struct with the values introduced in the command line and verifies errors
 * @param argc: number of arguments
 * @param argv: array of arguments
 * @param args: Arguments struct passed by reference
 * @return int: returns 0 if success; 1 otherwise
 */
int InitializeArguments(int argc, char *argv[], struct Arguments *args);

/**
 * Gets the value of the environment variable LOG_FILENAME
 * @return FILE*: file descriptor of the file indicated in the LOG_FILENAME environment variable; or NULL if error occurs
 */
FILE* GetRegistsFile();

int GetFilePermissions(const char *pathname);

int GetNewPermMask(char *new_mode);

int GetNewPermissions(int form_perm, char *new_mode);




