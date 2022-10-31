//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD

#include "include.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

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
    char buffer[BUFSIZE];
    int messageSize = 0;
    char actualPath[PATH_MAX+1];
    int bytesRead;

    //read client's message --
    while(bytesRead = read(clientSocket, buffer, sizeof(buffer)) > 0) {
        //if(buffer[bytesRead-1] == '\n') break;
        break;
    }
    check(bytesRead, "Recieve error");
    buffer[bytesRead-1] = 0; //remove the \n

    printf("Reveiced: %s\n", buffer);
    fflush(stdout);


    FILE *fp = fopen(actualPath, "r");
    if(fp == NULL) {

     return NULL;   
    }

    //read file and send them to client
    while((bytesRead = fread(buffer, 1, BUFSIZE, fp)) > 0){
        write(clientSocket, buffer, bytesRead);
    }
    close(clientSocket);
    fclose(fp);
    printf("closing connection\n");
    return NULL;

}

int main(int argc, char const *argv[])
{
    /* code */
    int serverSocket;
    int clientSocket;
    int addrSize;
    
    struct sockaddr_in clientAddr;

    check((serverSocket = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create socket");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;
    serverAddr.sin_port = htons(SERVERPORT);

    check(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)), "Bind Failed");
    check(listen(serverSocket, SERVER_BACKLOG), "Listen Failed");

    //Had to change stack size for each thread. Otherwize, id be getting a segementation fault error on my system.
    int stackSize = PTHREAD_STACK_MIN * 15;
    pthread_attr_t att;
    pthread_attr_init(&att);
    pthread_attr_setstacksize(&att, stackSize);

    while(1) {
        printf("Waiting for connections... \n");

        addrSize = sizeof(serverAddr);

        check(clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, (socklen_t*)&addrSize), "accept failed");
        printf("Connected\n");

        //do stuff with connection
        pthread_t thread;
        int *pclient = malloc(sizeof(int));
        *pclient = clientSocket;
        pthread_create(&thread, &att, handleConnection, pclient);
    }


    return 0;
}
