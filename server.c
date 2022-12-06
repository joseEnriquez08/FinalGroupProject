//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: This program simulates a jewelry shop by running a multithreaded server using a threadpool. Threadpool being an implementation where there is a set number of threads and you resuse them.
//              This threadpool in this program is split into 3 sets of threads. One set for shop assitants, one set for sofas, and one for the remaining spots in the waiting list.
//              The clients/connections represent the customers. The connections/clients are stored in a FIFO Queue. As the clients connect to the server, they are enqueued in a linkedList
//              implementation of a queue. As the shop assitants/threads become available, they dequeue the clients and handle them. If the shop assitant threads are busy and the amount of
//              clients falls in the range of sofa amount, the sofa threads will then handle the clients and ask if they want to leave or wait. If all the sofa threads are
//              occupied, then the remaining waiting room threads will handle the clients and ask if they want to leave or wait. The sofa and waiting room threads will only remove clients
//              from the queue if they choose to leave. This differs from the shop assitant threads that dequeue the clients to talk to them. If there is no room, the client will be booted.
//              The threads will never terminate, they either a.) handle a client, or b.) continuously check for available clients in the queue.
//              All shop assitant threads will be using the data stored in the array of structs called: "items". This is data from the text file cartier_catalog_.txt

// Command line arguments: a.out, Number of shop assitants, number of sofas, number of remaing waiting spots.

//TODO:
//      *fix bugs with sofa threads
//      *implement remiaing waiting room threads
//      *handle client responses for shop assitant threads
//
//Bugs: 
//      (1) Assume a client X gets connected to a sofa, and all assitant threads are occupied with other clients. If client x chooses to leave and then another client gets connected to the server.
//      that new client will be blocked(recieves no data from server). The server will then print out a message saying that a thread detected a null client node. Either "shop assitant detected null"
//      when calling deque() or "queue is null" when calling getNextNonHandleBySofa(). This should not be happening because im (supposedly) ensuring the threads dont continue until the queue is non-empty
//  
//   
// Notes: 
//      (1) Before a client connects to a sofa thread, the program works as intended. The shop assitants are handling the clients on a first come first serve basis. And the threads are being
//           resused


#include <stdbool.h>
#include "include.h"
#include "readFile.h"
#include "get_AllJewelfunc.h"
#include "get_Jewelfunc.h"
#include "purchase_Jewelfunc.h"
#include "return_Jewelfunc.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

//keeps track of threads and amount of clients
int clientAmount = 0;
int sofaInitializer = 0;
int shopAssitantInitializer = 0;
int waitingInitializer = 0;

// will be initialized by command line arguments
int shopAssitants;
int threadPoolSize;
int sofaAmount;
int waitingSpots;

//sofa + waiting spots
int maxWaitingRoom;

//lock to synchronize threads accessing the queue.
pthread_mutex_t assistantQueueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sofaQueueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitingQueueLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t sofaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shopAssitantLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t catalogLock = PTHREAD_MUTEX_INITIALIZER;

//condition variables let threads wait for some condition to occur
pthread_cond_t assitantConditionVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t sofaConditionVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitingConditionVar = PTHREAD_COND_INITIALIZER;

pthread_mutex_t *sofaOccupiedLocks;
pthread_cond_t *sofaOccupiedConditionVariables;

pthread_mutex_t *waitOccupiedLocks;
pthread_cond_t *waitOccupiedConditionVariables;

struct sockaddr_in serverAddr;
struct sockaddr SA;

//data to be shared among the threads
struct item *items;

//global queues
struct queue assitantQueue;
struct queue sofaQueue;
struct queue waitingRoomQueue;

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

/// @brief From research, Apparently the kernal will sometimes decide to not send all the data when reading and writing.
///         SO i wrote a function modeling the normal write/read functions that will make sure that full data gets sent,
///         This avoiding partial reads/writes. The loop will continuously send/recieve data until the number of bytes sent/received matches len
/// @param fd the socket to send data to
/// @param buf the buffer contaning the data to be sent
/// @param len the number of bytes that need to be sent
/// @return the number of bvytes sent
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

/// @brief From research, Apparently the kernal will sometimes decide to not send all the data when reading and writing.
///         SO i wrote a function modeling the normal write/read functions that will make sure that full data gets sent,
///         This avoiding partial reads/writes. The loop will continuously send/receive data until the number of bytes sent/received matches len
/// @param fd the socket to read data from
/// @param buf the buffer to store the data
/// @param len the number of bytes that need to be read
/// @return the number of bytes read
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

// @brief intitialize items array to keep track of a single clients' purchases/returns
// @param arr to be initialized
void initializeItemsArray(struct item arr[]){
    int size = 46;
    char buffer[BUFSIZE] = {0};
    sprintf(buffer, "0");

    //copies global store inventory to client inventory
    memcpy(arr, items, sizeof(struct item)*46);

    //sets all item's quanity in client inventory to 0 
    for(int i = 0; i < size; i++){
        strcpy(arr[i].quantity, buffer);
        arr[i].length = size;
    }
}

/// @brief handles clients that are connected to a shop assitant thread
/// @param pClientSocket //the client socket of client
/// @param shopAssitantId // the id of a shgop assitant
void *handleConnection(int pClientSocket, int shopAssitantID){
    int clientSocket = pClientSocket;
    //free(pClientSocket);
    char buffer[BUFSIZE] = {0};

    struct Catalog clientPurchases[46];
    //sets all item's quanity in client inventory to 0
    for(int i = 0; i < 46; i++){
        clientPurchases[i] = catalogstr[i];
        clientPurchases[i].quantity = 0;
    }

    //sends 0 to the client notifying them thaty are being handled by a shop assitant
    sprintf(buffer, "%d", 0);
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);

    //client output when shop assitant is free to handle client/customer
    sprintf(buffer, "Welcome to the shop..\n\n"
        "I am Shop Assistant #%d currently serving you.\n\n"
        "What would you like to do today? Please choose the option below:\n\n"
        "1. Looking at the jewelry menu\n"
        "2. Making specific jewelry inguiry\n"
        "3. Making purchase\n"
        "4. Returning the purchase\n"
        "5. Exit\n", shopAssitantID);

    //writing to client:
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //loop to conituously and infinitely read from client
    while(1){


        //reads client output
        fullread(clientSocket, buffer, sizeof(buffer));

        if(strcmp(buffer, "1") == 0){
            printf("Customer chose: 1. Looking at the jewelry menu\n");
            getAllJewelfunc(clientSocket);
            //printItems(items);
        }

        if(strcmp(buffer, "2") == 0){
            printf("Customer chose: 2. Making specific jewelry inguiry\n");
            getJewelfunc(clientSocket);
        }

        if(strcmp(buffer, "3") == 0){
            printf("Customer chose: 3. Making purchase\n");
            purchaseJewelfunc(clientSocket, clientPurchases);
            //printItems(clientPurchases);
        }

        if(strcmp(buffer, "4") == 0){
            printf("Customer chose: 4. Returning the purchase\n");
            returnJewelFunc(clientSocket, clientPurchases);
            //printItems(clientPurchases);
        }

        if(strcmp(buffer, "5") == 0){
            printf("Customer chose: 5. Exit\n");

            sprintf(buffer, "You have been removed from the shop\n");
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
			//check(fullWrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            close(clientSocket);
            printf("closing connection\n");
            clientAmount--;
            return NULL;
        }
        buffer[0] = 0;
        fflush(stdout);
    }

    close(clientSocket);
    printf("closing connection\n");
    return NULL;

}

/// @brief Thread function to allow shop assistant threads to  grab and handle clients. Notice the infinite loop. This allows the thread/shop assitant to be reused for more clients.
void *threadFuncShopAssitant(void *arg){

    //sets up a unique shop assitant id for each thread.
    //is synchronized by mutex to ensure the ids are unique.
    pthread_mutex_lock(&shopAssitantLock);
    shopAssitantInitializer++;
    int shopAssitantID = shopAssitantInitializer;
    pthread_mutex_unlock(&shopAssitantLock);

    //thread enters infinite loop of handling clients and checking for available clients
    while(1){

        struct node *clientNode = NULL;

        //synchronizes enqueueing and dequeueing by locking/unlocking the mutex
        pthread_mutex_lock(&assistantQueueLock);
        if((clientNode = dequeue(&assitantQueue)) == NULL){
            //no clients to handle

            //thread waits until a client is added to the queue
            pthread_cond_wait(&assitantConditionVar, &assistantQueueLock);

            //new client
            clientNode = dequeue(&assitantQueue);

        }
        
        pthread_mutex_unlock(&assistantQueueLock);

        if(clientNode == NULL){
            printf("Shop assitant detected null\n");
        }
        if(clientNode != NULL){
            //sets the state of the client
            clientNode->beingHandledByShopAssistant = 1;
            clientNode->beingHandledByRoom = 0;
            clientNode->beingHandledBySofa = 0;

            if(clientNode->sofaID > 0){
                printf("Signaling %d\n", clientNode->sofaID);
                pthread_cond_signal(&sofaOccupiedConditionVariables[clientNode->sofaID-1]);
            }

            //thread is handling the client until it disconnectes
            handleConnection(clientNode->clientSocket, shopAssitantID);
        }
    }
}

/// @brief removes a specific client from the queue. Will oly be used by sofa and waiting room threads
/// @param client pointer to the client to be removed
void removeClient(struct node *client){

    //notifying client that they have been removed
    int clientSocket = (client->clientSocket);
    char buffer[BUFSIZE] = {0};
    sprintf(buffer, "You have been removed from the shop\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);

    //decrements client amount and closes connection
    clientAmount--;
    close((client->clientSocket));
    printf("closing connection\n");

    //prints the amount of clients in comparison to threadpool size (used for debugging and testing)
    printf("%d <= %d\n", clientAmount, threadPoolSize);
    //prints the rest of the queue(used for debugging and testing)
    //printQ();
}

/// Thread function to allow sofa threads to grab and handle clients. Notice the infinite loop. This allows the thread/sofa to be reused for more clients.
void *threadFuncSofa(void *arg){

    //sets up a unique sofa id for each thread.
    //is synchronized by mutex to ensure the ids are unique.
    pthread_mutex_lock(&sofaLock);
    sofaInitializer++;
    int sofaID = sofaInitializer;
    pthread_mutex_unlock(&sofaLock);
   
    //thread enters infinite loop of handling clients and checking for available clients
    while(1){

        struct node *clientNode = NULL;
        pthread_mutex_lock(&sofaLock);

        if((clientNode = dequeue(&sofaQueue)) == NULL){
            //no clients need sofa

            //thread waits until a client is added to the queue
            pthread_cond_wait(&sofaConditionVar, &sofaQueueLock);

            //new client
            clientNode = dequeue(&sofaQueue);
        }
        
        pthread_mutex_unlock(&sofaLock);

        if(clientNode == NULL){
            //client node should never be null
            printf("Sofa Queue is null\n");
            continue;
        }
        

        if(clientNode != NULL){
            if(clientNode->waitID > 0){
                printf("Signaling %d\n", clientNode->waitID);
                pthread_cond_signal(&waitOccupiedConditionVariables[clientNode->waitID-1]);
            }

            int clientSocket = (clientNode->clientSocket);
            //sends 1 to the client notifying them they are being handled by a sofa
            char buffer[BUFSIZE] = {0};
            sprintf(buffer, "%d", 1);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            //client output when connecting to sofa
            sprintf(buffer, "All the shop assitants are currently busy assisting the customers.\n\n"
                "You are placed in the waiting room.\n\n"
                "Your current number in the waiting list is %d.\n\n"
                "Sofa #: %d\n\n"
                "Do you want to wait or want to leave the shop?\n"
                "Enter 0 for waiting or 1 to leave\n", assitantQueue.qSize+1, sofaID);

            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            
            fullread(clientSocket, buffer, sizeof(buffer));
            

            if(strcmp(buffer, "0") == 0){
                printf("CLient %d wants to wait, sofa: %d\n", clientNode->index+1, sofaID);
                pthread_mutex_lock(&assistantQueueLock);
                clientNode->sofaID = sofaID;
                //stores the client socket in the queue
                enqueueFromSofa(clientNode->clientSocket, sofaID, &assitantQueue);
                //enqueue(clientNode->clientSocket, &assitantQueue);
                pthread_mutex_unlock(&assistantQueueLock);
                printf("Waiting on sofaID: %d\n", sofaID);
                pthread_cond_wait(&sofaOccupiedConditionVariables[sofaID-1], &sofaOccupiedLocks[sofaID-1]);
                
                //TODO:  loop:
                //          will wait for a broad cast SIGNAL from the shop assitant thread()
                //          then check state of client node, if being handled by assitant, then break;
            }

            if(strcmp(buffer, "1") == 0){
                printf("CLient wants to leave, sofa: %d\n", sofaID);
                removeClient(clientNode);
                continue;
            }

        }
        
    }
}

 //////////////////////////////////////////////////////////////////////////////////////////

//Thread function to allow wait threads to grab and handle clients. Notice the infinite loop. This allows the thread/wait to be reused for more clients.
void *threadFuncWaiting(void *arg){

    //sets up a unique wait id for each thread.
    //is synchronized by mutex to ensure the ids are unique.
    pthread_mutex_lock(&waitLock);
    waitingInitializer++;
    int waitID = waitingInitializer;
    pthread_mutex_unlock(&waitLock);
   
    //thread enters infinite loop of handling clients and checking for available clients
    while(1){

        struct node *clientNode = NULL;
        pthread_mutex_lock(&waitLock);

        if((clientNode = dequeue(&waitingRoomQueue)) == NULL){
            //no clients need sofa

            //thread waits until a client is added to the queue
            pthread_cond_wait(&waitingConditionVar, &waitingQueueLock);

            //new client
            clientNode = dequeue(&waitingRoomQueue);
        }

        pthread_mutex_unlock(&waitLock);

        if(clientNode == NULL){
            //client node should never be null
            printf("Wait Queue is null\n");
            continue;
        }
        

        if(clientNode != NULL){

            int clientSocket = (clientNode->clientSocket);
            //sends 1 to the client notifying them they are being handled by a sofa
            char buffer[BUFSIZE] = {0};
            sprintf(buffer, "%d", 2);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            //client output when connecting to sofa
            sprintf(buffer, "All the shop assitants are currently busy assisting the customers.\n\n"
                "You are placed in the waiting room.\n\n"
                "Your current number in the waiting list is %d.\n\n"
                "All sofas are occupied.. Please stand in the waiting room..\n\n"
                "Do you want to wait or want to leave the shop?\n"
                "Enter 0 for waiting or 1 to leave\n", clientNode->index + assitantQueue.qSize+1);

            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            //stops and waits for client input
            // while(read(clientSocket, buffer, sizeof(buffer)) == 0){

            // }
            fullread(clientSocket, buffer, sizeof(buffer));
            printf("Reach\n");

            if(strcmp(buffer, "0") == 0){
                printf("Client %d wants to wait, Waiting: %d\n", clientNode->index+1, waitID);
                //pthread_mutex_lock(&sofaQueueLock);
                clientNode->waitID = waitID;
                //stores the client socket in the queue
                printf("Sofa queue size: %d\n", sofaQueue.qSize);
                enqueueFromWaiting(clientNode->clientSocket, waitID, &sofaQueue);
                printf("Sofa queue size: %d\n", sofaQueue.qSize);
                //enqueue(clientNode->clientSocket, &assitantQueue);
                //pthread_mutex_unlock(&sofaQueueLock);
                printf("Waiting on waitID: %d\n", waitID);
                pthread_cond_wait(&waitOccupiedConditionVariables[waitID-1], &waitOccupiedLocks[waitID-1]);
            }

            if(strcmp(buffer, "1") == 0){
                printf("CLient %d wants to leave, wait: %d\n", clientNode->index+1, waitID);

                //remove node
                removeClient(clientNode);
                continue;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////


/// @brief function to disconnect client when there is no room
/// @param clientSocket the client socket
void disconnectClientDueToFullRoom(int clientSocket){
    //sends the client 3 to tell them they are being disconnected
    char buffer[BUFSIZE] = {0};
    sprintf(buffer, "%d", 3);
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);


    //sends goodbye messade to client
    sprintf(buffer, "Sorry, the waiting room is full.. We will not be able to serve you at the moment.\n\n"
                "Please check back later.. Thank you..");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);

    //closes client connection
    close(clientSocket);
    //free(clientSocket);
    printf("closing connection\n");

}

/// @brief creates the multithreaded server
/// @param argc amount of command line arguments
/// @param argv array of command line arguments
int main(int argc, char const *argv[])
{
    //initialize the data to be shared among the threads
//     items = parseData();
    assitantQueue.qSize = 0;
    sofaQueue.qSize = 0;
    waitingRoomQueue.qSize = 0;

    // Reading data and keeping in catalog struct
    if(readFile()==-1){
      printf("Error in Reading Cartier_catalog.txt\n");exit(1);
    }

    //command line arguments
    shopAssitants = strtol(argv[1], NULL, 10);
    sofaAmount = strtol(argv[2], NULL, 10);
    maxWaitingRoom = strtol(argv[3], NULL, 10);
    waitingSpots = maxWaitingRoom - sofaAmount;
    threadPoolSize = shopAssitants + maxWaitingRoom;

    sofaOccupiedLocks = calloc(sofaAmount, sizeof(pthread_mutex_t));
    sofaOccupiedConditionVariables = calloc(sofaAmount, sizeof(pthread_cond_t));

    waitOccupiedLocks = calloc(waitingSpots, sizeof(pthread_mutex_t));
    waitOccupiedConditionVariables = calloc(waitingSpots, sizeof(pthread_cond_t));

    for(int i = 0; i < sofaAmount; i++){
        pthread_mutex_init(&sofaOccupiedLocks[i], NULL);
        pthread_cond_init(&sofaOccupiedConditionVariables[i], NULL);
    }

    for(int i = 0; i < waitingSpots; i++){
        pthread_mutex_init(&waitOccupiedLocks[i], NULL);
        pthread_cond_init(&waitOccupiedConditionVariables[i], NULL);
    }

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
    for(int i = 0; i < shopAssitants; i++){
        pthread_create(&threadPool[i], &att, threadFuncShopAssitant, NULL);
    }

    //creates threads based on the amount of sofas
    for(int i = shopAssitants; i < shopAssitants + sofaAmount; i++){
        pthread_create(&threadPool[i], &att, threadFuncSofa, NULL);
    }

    //creates threads based on the amount of waiting spots that arnt sofas
    for(int i = sofaAmount; i < maxWaitingRoom; i++){
        pthread_create(&threadPool[i], &att, threadFuncWaiting, NULL);
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
        //int *pclient = malloc(sizeof(int));
        //*pclient = clientSocket;

        if(clientAmount >= threadPoolSize){
            //there is no space available in the shop, need to disconnect client.
            printf("%d > %d\n", clientAmount, threadPoolSize);
            disconnectClientDueToFullRoom(clientSocket);

        } else {
            //there is space available in the shop

            
            //beacause all threads are sharing the same queue structure, we have to sycnhronize enqueuing and dequeueing
            //pthread_mutex_lock(&queueLock);

            //clientAmount++;
            //prints the amount of clients in comparison to threadpool size (used for debugging and testing)
            printf("%d <= %d\n", clientAmount, threadPoolSize);
            

            //prints the rest of the queue(used for debugging and testing)
           // printQ();
            
            
            if(clientAmount < shopAssitants){
                pthread_mutex_lock(&assistantQueueLock);
                clientAmount++;
                //stores the client socket in the queue
                enqueue(clientSocket,&assitantQueue);
                printQ(&assitantQueue);
                pthread_mutex_unlock(&assistantQueueLock);

                //signals shop assitant that there is a new client
                pthread_cond_signal(&assitantConditionVar);
            }


            else if(assitantQueue.qSize < sofaAmount){

                pthread_mutex_lock(&sofaQueueLock);
                clientAmount++;
                //stores the client socket in the queue
                enqueue(clientSocket, &sofaQueue);
                printQ(&sofaQueue);
                pthread_mutex_unlock(&sofaQueueLock);

                //client needs a sofa, so a signal is sent to a shop assitant thread
                pthread_cond_signal(&sofaConditionVar);
            }

            else {//if(assitantQueue.qSize = sofaAmount && assitantQueue.qSize <= maxWaitingRoom){

                pthread_mutex_lock(&waitingQueueLock);
                clientAmount++;
                //stores the client socket in the queue
                enqueue(clientSocket, &waitingRoomQueue);
                printf("Waiting room queue size %d\n", waitingRoomQueue.qSize);
                pthread_mutex_unlock(&waitingQueueLock);

                //client needs a sofa, so a signal is sent to a shop assitant thread
                pthread_cond_signal(&waitingConditionVar);
            }

            //pthread_mutex_unlock(&queueLock);
        }
        

    }


    return 0;
}