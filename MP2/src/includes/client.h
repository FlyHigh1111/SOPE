#pragma once

#include "./common.h"
#define CLIENT -1
#define MAX_THREADS 10000


pthread_mutex_t lock1; //contains the mutex's state (opened ou closes)
pthread_mutex_t lock2;
int errno;
int cont; //counter of number of threads created by the main thread.
bool finish = true;
int fdt[MAX_THREADS] = {0}; 

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
 * @brief Struct with the arguments use in function ThreadHandler().
 */
struct ArgsThread
{
    pid_t pid; //process id of the program
    int fd_public_fifo; //file descriptor of the public FIFO
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
 * @param seed: seed 
 * @return int: random number created
 */
int Randomize(int lower, int upper, unsigned int seed);

/**
 * @brief Handles each thread
 * 
 * @param arguments: thread arguments defined in ThreadArgs
 * @return void* 
 */
void* ThreadHandler(void *arguments);

/**
 * @brief Writes information kept in Log struct in stdout
 * 
 * @param log: information to write
 */
void WriteLog(struct Log log);

/**
 * @brief Handles the signal SIGALRM
 */
void sigAlrmHandler();