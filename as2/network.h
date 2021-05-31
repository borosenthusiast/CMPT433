/*Handles and processes the UDP requests from netcat, including the stop operation, which calls each module's stop thread function.*/
#ifndef NETWORK_H
#define NETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
#include "bubble.h"
#include "bbg.h"

#define MSG_MAX_LEN 1500
#define PORT        12345


enum instructions{GET, COUNT, HELP, STOP};

void printArray(int*, int);

void* Listener_listen(void*);

void Listener_stopListen();

#endif