//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: common file for rest of the files on this program
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>    
#include <errno.h>      
#include <sys/ipc.h>    
#include <sys/msg.h>   
#include <string.h>   
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <pthread.h>
#include <limits.h>
#include <poll.h>


#define SERVERPORT 1024
#define BUFSIZE 4086


//model for client node
struct node {
    struct node* next;
    int *clientSocket;
    //id
    //int size
    int index;
    int beingHandledByShopAssistant;
    int beingHandledBySofa;
    int beingHandledByRoom;
};

//dynamic array
struct Array{
    char str[50];
    int length;
};

//model for items
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

//global array of structs to keep the entire inventory
extern struct item *items;

//global functions
extern struct item* parseData();
extern void enqueue(int *clientSocket);
extern struct node* dequeue();
extern int getQueueSize();
extern struct node* getNextNonHandleBySofa();
extern struct node* peek();
extern int getQueueSize();
extern int removeNode(int index);
extern void printQ();

extern void getAllItems(int clientSocket);
extern void getItemInfo(int clientSocket);
extern void makePurchase(int clientSocket, struct item clientPurchases[]);
extern void returnItem(int clientSocket, struct item clientPurchases[]);

extern int check(int exp, const char *message);
extern void printItems(struct item *items);


extern ssize_t fullwrite(int fd, const void *buf, size_t len);
extern ssize_t fullread(int fd, void *buf, size_t len);





