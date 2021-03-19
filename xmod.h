#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/times.h>
#include <strings.h>
#include <ctype.h>

struct Arguments
{
    int mode_octal;     //mode introduced in octal
    char *mode;         //mode introduced in the format <u|g|o|a><-|+|=><rwx>
    bool mode_is_octal; //true if mode is in octal; false otherwise
    char *path_name;    //path with the directory/file 
    bool option_v;      //true if mode '-v' was introduced
    bool option_c;      //true if mode '-c' was introduced
    bool option_R;      //true if mode '-R' was introduced
};

static void signal_func(int);

void PrintError(int error);

FILE* GetRegistsFile();

void InitializeArguments(int argc, char *argv[], struct Arguments *args);

int GetFilePermissions(const char *path_name);

int GetNewPermMask(char *new_mode);

int GetNewPermissions(int form_perm, char *new_mode);

void oct_to_mode(int octal, char *mode);

void ChangePermissions(const struct Arguments *args, char *path);

void ProcessRecursive(int argc, char *argv[], char *envp[], const struct Arguments *args);

bool WriteSignalInfo(bool handler_flag, const struct Arguments *args);

void Initialize_child_handler();

void Initialize_main_handler();
