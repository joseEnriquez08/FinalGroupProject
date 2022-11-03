//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD

#include "include.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100
#define THREAD_POOL_SIZE 20

pthread_t threadPool[THREAD_POOL_SIZE];
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;

//condition variables let threads wait for some condition to occur
pthread_cond_t conditionVar = PTHREAD_COND_INITIALIZER;

struct sockaddr_in serverAddr;
struct sockaddr SA;

int check(int exp, const char *message){
    if(exp == SOCKETERROR){
        perror(message);
        exit(1);
    }

    return exp;
}

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

void *threadFunc(void *arg){
    while(1){
        int *pclient;
        pthread_mutex_lock(&queueLock);
        

        if((pclient = dequeue()) == NULL){
            //no clients to handle

            //so thread waits
            pthread_cond_wait(&conditionVar, &queueLock);
            pclient = dequeue();
        }
        pthread_mutex_unlock(&queueLock);

        if(pclient != NULL){
            handleConnection(pclient);
        }
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    int serverSocket;
    int clientSocket;
    int addrSize;
    
    struct sockaddr_in clientAddr;

    //Had to change stack size for each thread. Otherwize, id be getting a segementation fault error on my system.
    int stackSize = PTHREAD_STACK_MIN * 15;
    pthread_attr_t att;
    pthread_attr_init(&att);
    pthread_attr_setstacksize(&att, stackSize);

    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        pthread_create(&threadPool[i], &att, threadFunc, NULL);
    }

    check((serverSocket = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create socket");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVERPORT);

    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

    check(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)), "Bind Failed");
    check(listen(serverSocket, SERVER_BACKLOG), "Listen Failed");

    

    while(1) {
        printf("Waiting for connections... \n");

        addrSize = sizeof(serverAddr);

        check(clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, (socklen_t*)&addrSize),
            "accept failed");
        printf("Connected\n");

        //put connection somewhere to use it latyer
        int *pclient = malloc(sizeof(int));
        *pclient = clientSocket;
        pthread_mutex_lock(&queueLock);
        enqueue(pclient);
        pthread_cond_signal(&conditionVar);
        pthread_mutex_unlock(&queueLock);

        //do stuff with connection
        // pthread_t thread;
        // int *pclient = malloc(sizeof(int));
        // *pclient = clientSocket;
        // pthread_create(&thread, &att, handleConnection, pclient);
    }


    return 0;
}
