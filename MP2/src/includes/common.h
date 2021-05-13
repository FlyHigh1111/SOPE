#pragma once

#include "./lib.h"

#define BUFF_SIZE 1024;
#define BUFFER_SIZE 256
/**
 * @brief Struct for exchange of messages between client and server
 */
struct Message{
	int rid; 		// request id
	pid_t pid; 	    // process id
	pthread_t tid;	// thread id
	int tskload;    // task load
	int tskres;		// task result
};