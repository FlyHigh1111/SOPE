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

#define CLIENTE -1
#define MAX_THREADS 10000
#define BUFFER_SIZE 256

pthread_mutex_t lock; //contains the mutex's state (opened ou closes)
int errno;
int cont; //counter of number of threads created by the main thread.

/**
 * @brief Struct with the arguments introduced in the command line.
 */
struct Arguments
{
    size_t nsecs;      //number of seconds
    char *public_fifo; //public FIFO
};

/**
 * @brief Struct with information to print to stdout
 */
struct Log 
{
    time_t inst;   //return value of time() function
    int i;         //unique request number
    int t;         //task load
    pid_t pid;     //process ID
    pthread_t tid; //thread ID
    int res;       //task result
    char *oper;    //operation made
};

/**
 * @brief Struct with the arguments use in function thread_handle().
 */
struct ArgsThread
{
    pid_t pid; //process id of the program
    int fd_public_fifo; //file descriptor of the public FIFO
};

/**
 * @brief Struct for exchange of messages between client and server
 */
struct Message 
{
	int rid;	// request id
	pid_t pid;	// process id
	pthread_t tid;	// thread id
	int tskload;	// task load
	int tskres;	// task result
};

/**
 * @brief Parses arguments introduced in the command line and initializes the struct Arguments
 * @param argc: number of arguments
 * @param argv: array of chars with the arguments introduced
 * @param args: struct Arguments to initialize
 */
void ParseArguments(int argc, char *argv[], struct Arguments *args);

/**
 * @brief Creates a random number between lower and upper.
 * 
 * @param lower: lower number
 * @param upper: upper number
 * @return int: random number created
 */
int Randomize(int lower, int upper);

void* ThreadHandler(void *arguments);

void WriteLog(struct Log log);