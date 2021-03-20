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

/**
 * @brief Struct with informations about the commands introduced in the command line
 * 
 */
struct Arguments
{
    int mode_octal;     // mode introduced in octal 
    char *mode;         // mode introduced in the format <u|g|o|a><-|+|=><rwx> 
    bool mode_is_octal; // true if mode is in octal; false otherwise 
    char *path_name;    // path with the directory/file 
    bool is_dir;        // true is path_name is a directory 
    bool option_v;      // true if mode '-v' was introduced 
    bool option_c;      // true if mode '-c' was introduced 
    bool option_R;      // true if mode '-R' was introduced 
};

struct Arguments args; //!< Arguments struct
int nfmod = 0; //!< number of files already modified at the moment
int nftot = 0; //!< number of files founded at the moment
struct timeval start, end;
struct timeval curr_time; 

/**
 * @brief Prints a mini-manual when the user types "./xmod.o --help"
 * 
 */
void PrintManual();

/**
 * @brief Prints a message with the type of error occurred. 
 * 
 * @param error: number of the error 
 */
void PrintError(int error);//Prints Every Error Possible

/**
 * @brief Gets the file of regists.
 * Gets the value of the enviroment variable LOG_FILENAME, opens the file and returns its file descriptor
 * 
 * @return FILE*: file descriptor
 */
FILE* GetRegistsFile();

/**
 * @brief Writes various information to the regists file.
 * 
 * @param regists_file: the regists file descriptor to where it writes the information 
 * @param time: time passed since the beginning of the program
 * @param pid: process' id 
 * @param event: event occurred
 * @param info: info about event
 */
void WriteLog(FILE *regists_file, double time, int pid, char event[], char info[]);

/**
 * @brief Initializes the Arguments structure
 * 
 * @param argc: number of arguments introduced
 * @param argv: array of chars with the arguments introduced
 * @param args: the Arguments struct
 */
void InitializeArguments(int argc, char *argv[], struct Arguments *args);

/**
 * @brief Gets the original file permissions
 * 
 * @param path_name: path name of the file/directory
 * @return int: the permissions in octal 
 */
int GetFilePermissions(const char *path_name);//Returns the original Permissions that the file path_name has 

/**
 * @brief Transforms mode into a mask 
 * 
 * @param new_mode 
 * @return int 
 */
int GetNewPermMask(char *new_mode);

/**
 * @brief Determines new permissions according to the mode and new mask
 * 
 * @param form_perm 
 * @param new_mode 
 * @return int: the new permissions
 */
int GetNewPermissions(int form_perm, char *new_mode);

/**
 * @brief Converts mode from octal to mode in format <u|g|o|a><-|+|=><rwx> 
 * 
 * @param octal: the mode in octal 
 * @param mode: the mode to obtain
 */
void OctalToMode(int octal, char *mode);

/**
 * @brief Changes the permissions of the file/directory given using chmod() function.
 * 
 * @param args: arguments structure
 * @param path: file/directory which the permissions are changed
 */
void ChangePermissions(const struct Arguments *args, char *path);

/**
 * @brief Exit handler
 * 
 * @param signo 
 */
static void ExitHandler(int signo);

/**
 * @brief Processes the given directory recursively using execve.
 * 
 * @param argc: number og arguments introduced
 * @param argv: array of chars with the arguments introduced
 * @param envp: array of chars with all the system enviroment variables
 * @param args: Arguments structure
 */
void ProcessRecursive(int argc, char *argv[], char *envp[], const struct Arguments *args, FILE *regists_file, struct timeval start, struct timeval curr_time);

/**
 * @brief Signal handler
 * Receives a termination signal sent by the user by pressing CTRL-C. 
 * This funtion manipulates this signal and presents:
 * - the processes in execution, 
 * - the current directory, 
 * - the number of files founded at the moment and 
 * - the number of files already modified
 * In the end, it asks the user if he wnats to terminate ou continue. Regists the response and acts accordingly. 
 */
static void SignalFunc(int);

/**
 * @brief Main program where the flow of execution happens
 * 
 * @param argc: number og arguments introduced
 * @param argv: array of chars with the arguments introduced
 * @param envp: array of chars with all the system enviroment variables
 * @return int 
 */
int main(int argc, char *argv[], char *envp[]);
