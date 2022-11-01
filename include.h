//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>    // for wait
#include <errno.h>      // for system error numbers
#include <sys/ipc.h>    // Used by IPC messages, shared memory and semaphores
#include <sys/msg.h>    // for message queue structures
#include <string.h>     // for string operation: strlen
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <pthread.h>
#include <limits.h>
//#include <sys/pthreads.h>

#define SERVERPORT 1024
#define BUFSIZE 4086

struct node {
    struct node* next;
    int *clientSocket;
    //int index
};

struct Array{
    char str[50];
    int length;
};

//ref	category	title	tags	description	quantity	price
struct item{
    char ref[50];
    char category[50];
    char title[50];
    char tags[50];
    char description[255];
    char quantity[5];
    char price[10];
    int length;
};

//global functions
extern struct item* parseData();
extern void enqueue(int *clientSocket);
extern int* dequeue();




