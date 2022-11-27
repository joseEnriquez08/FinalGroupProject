#include "include.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

//keeps track of threads and amount of clients
int clientAmount = 0;
int sofaInitializer = 0;
int shopAssitantInitializer = 0;

// will be initialized by command line arguments
int shopAssitants;
int sofaAmount;
int waitingSpots;

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

ssize_t fullwrite(int fd, const void *buf, size_t len){
	size_t total = 0;
	const char *p = buf;
	while(total < len){
		ssize_t r = write(fd, p + total, len -total);
		switch (r) {
		case -1: return -1;
		case 0: return total;
		default: total += r;
	
		}
	}
	return total;
}

ssize_t fullread(int fd, void *buf, size_t len){
	size_t total = 0;
	char *p = buf;
	while(total < len){
		ssize_t r = read(fd, p+total, len-total);
		switch(r){
			case -1: return -1;
			case 0: return total;
			default: total += r;
		}
	}

	return total;
}


/// @brief handles clients that are connected to a shop assitant thread
/// @param pClientSocket //the client socket of client
/// @param shopAssitantId // the id of a shgop assitant
void *handleConnection(void* pClientSocket, int shopAssitantID){
    int clientSocket = *((int*)pClientSocket);
    free(pClientSocket);
    char buffer[BUFSIZE] = {0};

    //loop to conituously and infinitely read from client
    while(1){

        //stops and waits for client input from client
        // while( == 0){

        // }
        fullread(clientSocket, buffer, sizeof(buffer));

        if(strcmp(buffer, "1") == 0){
            printf("Customer chose: 1. Looking at the jewelry menu\n");

            //loop to send data.
            for(int i = 0; i< items->length; i++){
                sprintf(buffer, "Item %d: %s\t%s\t%s\t%s\t%s\t%s\n", i,
                    items[i].ref, items[i].title, items[i].tags,
                    items[i].description, items[i].quantity, items[i].price);

                printf("sending: %s", buffer);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            }
        }

        buffer[0] = 0;
        fflush(stdout);
    }

    close(clientSocket);
    printf("closing connection\n");
    return NULL;

}
struct item* fillItems(){
    //decaltres appropriate sized array of structs
    int size = 46;
    struct item *items = calloc(size, sizeof(struct item));
    char buffer[50] = {0};

    for(int i = 0; i < size; i++){
        sprintf(buffer, "ref%d", i);
        strcpy(items[i].ref, buffer);

        sprintf(buffer, "catgory%d", i);
        strcpy(items[i].category, buffer);

        sprintf(buffer, "title%d", i);
        strcpy(items[i].title, buffer);

        sprintf(buffer, "descript%d", i);
        strcpy(items[i].description, buffer);

        sprintf(buffer, "q%d", i);
        strcpy(items[i].quantity, buffer);

        sprintf(buffer, "price%d", i);
        strcpy(items[i].price, buffer);

        sprintf(buffer, "tags%d", i);
        strcpy(items[i].tags, buffer);

        items->length = size;
    }

    return items;
}

/// @brief creates the multithreaded server
/// @param argc amount of command line arguments
/// @param argv array of command line arguments
int main(int argc, char const *argv[])
{
    //initialize the data to be shared among the threads
    items = fillItems();

    //command line arguments
    shopAssitants = strtol(argv[1], NULL, 10);
    sofaAmount = strtol(argv[2], NULL, 10);
    waitingSpots = strtol(argv[3], NULL, 10);

    int serverSocket;
    int clientSocket;
    int addrSize;
    
    struct sockaddr_in clientAddr;

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

        handleConnection(pclient, 1);

    }
    return 0;
}


