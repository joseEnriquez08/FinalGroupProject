//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: This program runs a multithreaded server using a threadpool. Threadpool being an implementation where there is a set number of threads and you resuse them.
//              The idea is to have the shop assistants represent the threads, and clients/connections represent the customers. The connections/clients are stored in a
//              FIFO Queue. As the clients connect to the server, they are enqueued. As the shop assitants/threads become available, they dequeue the clients and handle them.
//              The threads will never terminate, they either a.) handle a client, or b.) continuously check for available clients in the queue.
//              All threads will be using the data stored in the array of structs called: "items". This ius data from the text file cartier_catalog_.txt

#include "include.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100
//#define threadPoolSize 20

// will be initialized by command line arguments
int shopAssitants;
int threadPoolSize;
int sofaAmount;
int maxCustomers;

//lock to synchronize threads accessing the queue.
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;

//condition variables let threads wait for some condition to occur
pthread_cond_t conditionVar = PTHREAD_COND_INITIALIZER;

struct sockaddr_in serverAddr;
struct sockaddr SA;

//data to be shared among the threads
struct item *items;

/// @brief function to aid in debugging bu printing error messages
/// @param exp numeric result of a function
/// @param message accompanying message to be printed if the numeric reuslt implies error.
/// @return exp numeric result of a function
int check(int exp, const char *message){
    if(exp == SOCKETERROR){
        perror(message);
        exit(1);
    }

    return exp;
}

/// @brief 
/// @param pClientSocket 
/// @return 
void *handleConnection(void* pClientSocket){
    int clientSocket = *((int*)pClientSocket);
    free(pClientSocket);
    char buffer[BUFSIZE] = {0};
    int messageSize = 0;
    char actualPath[PATH_MAX+1];
    int bytesRead;

    //client output when shop assitant/thread is free to handle client/customer
    sprintf(buffer, "Welcome to the shop..\n\n"
        "I am Shop Assistant #%d currently serving you.\n\n"
        "What would you like to do today? Please choose the option below:\n\n"
        "1. Looking at the jewelry menu\n"
        "2. Making specific jewelry inguiry\n"
        "3. Making purchase\n"
        "4. Returning the purchase\n"
        "5. Exit\n", 1);

    //printf("%s", buffer);

    //writing to client:
    check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    while(1){

        //reading from client
        while(read(clientSocket, buffer, sizeof(buffer)) == 0){

        }

        if(strcmp(buffer, "1") == 0){
            printf("Customer chose: 1. Looking at the jewelry menu\n");
            
        }

        if(strcmp(buffer, "2") == 0){
            printf("Customer chose: 2. Making specific jewelry inguiry\n");
        }

        if(strcmp(buffer, "3") == 0){
            printf("Customer chose: 3. Making purchase\n");
        }

        if(strcmp(buffer, "4") == 0){
            printf("Customer chose: 4. Returning the purchase\n");
        }

        if(strcmp(buffer, "5") == 0){
            printf("Customer chose: 5. Exit\n");
            close(clientSocket);
            printf("closing connection\n");
            return NULL;
            
        }
        buffer[0] = 0;
        fflush(stdout);
    }
    //check(read(clientSocket, buffer, sizeof(buffer)), "Recieving/Reading failed");
    //printf("%s", buffer);

    
    //read client's message --
    //while(bytesRead = read(clientSocket, buffer, sizeof(buffer)) == 0) {
        //if(buffer[bytesRead-1] == '\n') break;
        //break;
    //}
    //check(bytesRead, "Recieve error");
    //buffer[bytesRead-1] = 0; //remove the \n

    //printf("Reveiced: %s\n", buffer);
    //fflush(stdout);

    //realpath(buffer, actualPath);
    //FILE *fp = fopen(actualPath, "r");
    //if(fp == NULL) {

    // return NULL;   
    //}

    //read file and send them to client
    //while((bytesRead = fread(buffer, 1, BUFSIZE, fp)) > 0){
    //    write(clientSocket, buffer, bytesRead);
    //}
    close(clientSocket);
    //fclose(fp);
    printf("closing connection\n");
    return NULL;

}

/// @brief Thread function to grab and handle clients. Notice the infinite loop. This allows the thread/shop assitant to be reused for more clients.
/// @param arg 
void *threadFunc(void *arg){

    //thread enters infinite loop of handling clients and checking for available clients
    while(1){

        int *pclient;
        //synchronizes enqueueing and dequeueing by locking/unlocking the mutex
        pthread_mutex_lock(&queueLock);

        if((pclient = dequeue()) == NULL){
            //no clients to handle

            //thread waits until a client is added to the queue
            pthread_cond_wait(&conditionVar, &queueLock);

            //new client
            pclient = dequeue();
        }
        pthread_mutex_unlock(&queueLock);

        if(pclient != NULL){
            //thread is handling the client until it disconnectes
            handleConnection(pclient);
        }
    }
}

/// @brief creates the multithreaded server
/// @param argc amount of command line arguments
/// @param argv array of command line arguments
int main(int argc, char const *argv[])
{
    //initialize the data to be shared among the threads
    items = parseData();

    //command line arguments
    shopAssitants = strtol(argv[1], NULL, 10);
    sofaAmount = strtol(argv[2], NULL, 10);
    maxCustomers = strtol(argv[3], NULL, 10);
    threadPoolSize = shopAssitants + sofaAmount + maxCustomers;

    pthread_t threadPool [threadPoolSize];

    int serverSocket;
    int clientSocket;
    int addrSize;
    
    struct sockaddr_in clientAddr;

    //Had to change stack size for each thread. Otherwize, id be getting a segementation fault error on my system.
    int stackSize = PTHREAD_STACK_MIN * 15;
    pthread_attr_t att;
    pthread_attr_init(&att);
    pthread_attr_setstacksize(&att, stackSize);

    //creates threads based on the amount of shop assitants
    for(int i = 0; i < threadPoolSize; i++){
        pthread_create(&threadPool[i], &att, threadFunc, NULL);
    }


    //creates server socket
    check((serverSocket = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create socket");

    //fills information about server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVERPORT);

    //allows program to reuse the socket it the socket is already in use
    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

    //binds the serve socket and sets it to listen for clients
    check(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)), "Bind Failed");
    check(listen(serverSocket, SERVER_BACKLOG), "Listen Failed");

    //inifinite loop to continuously accept and handle clients
    while(1) {
        printf("Waiting for connections... \n");

        addrSize = sizeof(serverAddr);

        //accepts a client socket
        check(clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, (socklen_t*)&addrSize),
            "accept failed");
        printf("Connected\n");

        //stores client socket in pointer so we can queue it.
        int *pclient = malloc(sizeof(int));
        *pclient = clientSocket;

        // if(getQueueSize() > ()){

        // }
        //beacause all threads are sharing the same queue structure, we have to sycnhronize enqueuing and dequeueing
        pthread_mutex_lock(&queueLock);

        //stores the client socket in the queue
        enqueue(pclient);
        pthread_cond_signal(&conditionVar);
        pthread_mutex_unlock(&queueLock);

    }


    return 0;
}
