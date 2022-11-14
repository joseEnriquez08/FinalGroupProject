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


#include "include.h"


#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

//keeps track of threads and amount of clients
int clientAmount = 0;
int sofaInitializer = 0;
int shopAssitantInitializer = 0;

// will be initialized by command line arguments
int shopAssitants;
int threadPoolSize;
int sofaAmount;
int waitingSpots;

//lock to synchronize threads accessing the queue.
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sofaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shopAssitantLock = PTHREAD_MUTEX_INITIALIZER;

//condition variables let threads wait for some condition to occur
pthread_cond_t assitantConditionVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t sofaConditionVar = PTHREAD_COND_INITIALIZER;

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

/// @brief handles clients that are connected to a shop assitant thread
/// @param pClientSocket //the client socket of client
/// @param shopAssitantId // the id of a shgop assitant
void *handleConnection(void* pClientSocket, int shopAssitantID){
    int clientSocket = *((int*)pClientSocket);
    free(pClientSocket);
    char buffer[BUFSIZE] = {0};

    //sends 0 to the client notifying them thaty are being handled by a shop assitant
    sprintf(buffer, "%d", 0);
    check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
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
    check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //loop to conituously and infinitely read from client
    while(1){

        //stops and waits for client input from client
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

            sprintf(buffer, "You have been removed from the shop\n");
            check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
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
        pthread_mutex_lock(&queueLock);
        if((clientNode = dequeue()) == NULL){
            //no clients to handle

            //thread waits until a client is added to the queue
            pthread_cond_wait(&assitantConditionVar, &queueLock);

            //new client
            clientNode = dequeue();

        }
        
        pthread_mutex_unlock(&queueLock);

        if(clientNode == NULL){
            printf("Shop assitant detected null\n");
        }
        if(clientNode != NULL){
            //sets the state of the client
            clientNode->beingHandledByShopAssistant = 1;
            clientNode->beingHandledByRoom = 0;
            clientNode->beingHandledBySofa = 0;

            //thread is handling the client until it disconnectes
            handleConnection(clientNode->clientSocket, shopAssitantID);
        }
    }
}

/// @brief removes a specific client from the queue. Will oly be used by sofa and waiting room threads
/// @param client pointer to the client to be removed
void removeClient(struct node *client){
    
    //removes node from client 
    removeNode(client->index);

    //notifying client that they have been removed
    int clientSocket = *(client->clientSocket);
    char buffer[BUFSIZE] = {0};
    sprintf(buffer, "You have been removed from the shop\n");
    check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);

    //decrements client amount and closes connection
    clientAmount--;
    close(*(client->clientSocket));
    printf("closing connection\n");

    //prints the amount of clients in comparison to threadpool size (used for debugging and testing)
    printf("%d <= %d\n", clientAmount, threadPoolSize);
    //prints the rest of the queue(used for debugging and testing)
    printQ();
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

        //old idea
        // //if(peek() == NULL || getQueueSize() <= sofaAmount){ //clientAmount <= shopAssitants || clientAmount > sofaAmount + shopAssitants){
        //     //queue is empty or there are no clients that need a sofa.
        //     //the "getQueueSize() <= sofaAmount" part may be a problem

        //     printf("q size: %d\n", getQueueSize());
            
        //     //wait until a client needs a sofa
        //     pthread_cond_wait(&sofaConditionVar, &sofaLock);
        // //}
        
        //new idea: sofa thread will wait by default. Will be signaled to run
        printf("q size: %d\n", getQueueSize());
        pthread_cond_wait(&sofaConditionVar, &sofaLock);
        clientNode = getNextNonHandleBySofa();
        pthread_mutex_unlock(&sofaLock);
        
     
        pthread_mutex_lock(&queueLock);
        //clientNode = getNextNonHandleBySofa();
        int clientSocket;

        if(clientNode == NULL){
            //for some reason this is printint. It should never print
            //client node should never be null
            printf("Queue is null\n");
            continue;
        }

        //printf("CLient INdex: %d\n", clientNode->index+1);
        if(clientNode != NULL){
            //sets the state of the client node 
            
            clientSocket = *(clientNode->clientSocket);
            clientNode->beingHandledBySofa = 1;
            clientNode->beingHandledByRoom = 0;
            clientNode->beingHandledByShopAssistant = 0;
        }
        
        pthread_mutex_unlock(&queueLock);

        if(clientNode != NULL){

            //sends 1 to the client notifying them they are being handled by a sofa
            char buffer[BUFSIZE] = {0};
            sprintf(buffer, "%d", 1);
            check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            //client output when connecting to sofa
            sprintf(buffer, "All the shop assitants are currently busy assisting the customers.\n\n"
                "You are placed in the waiting room.\n\n"
                "Your current number in the waiting list is %d.\n\n"
                "Sofa #: %d\n\n"
                "Do you want to wait or want to leave the shop?\n"
                "Enter 0 for waiting or 1 to leave\n", clientNode->index+1, sofaID);

            check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
            fflush(stdout);

            //stops and waits for client input
            while(read(clientSocket, buffer, sizeof(buffer)) == 0){

            }
            printf("Reach\n"); 

            if(strcmp(buffer, "0") == 0){
                printf("CLient %d wants to wait, sofa: %d\n", clientNode->index+1, sofaID);
                //TODO:  loop:
                //          will wait for a broad cast SIGNAL from the shop assitant thread()
                //          then check state of client node, if being handled by assitant, then break;
            }

            if(strcmp(buffer, "1") == 0){
                printf("CLient %d wants to leave, sofa: %d\n", clientNode->index+1, sofaID);

                //remove node
                pthread_mutex_lock(&queueLock);
                removeClient(clientNode);
                pthread_mutex_unlock(&queueLock);
                continue;
            }

        }
        
    }
}

/// @brief function to disconnect client when there is no room
/// @param clientSocket the client socket
void disconnectClientDueToFullRoom(int *clientSocket){
    //sends the client 3 to tell them they are being disconnected
    char buffer[BUFSIZE] = {0};
    sprintf(buffer, "%d", 3);
    check(write(*clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);


    //sends goodbye messade to client
    sprintf(buffer, "Sorry, the waiting room is full.. We will not be able to serve you at the moment.\n\n"
                "Please check back later.. Thank you..");
    check(write(*clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    fflush(stdout);

    //closes client connection
    close(*clientSocket);
    free(clientSocket);
    printf("closing connection\n");

}

/// @brief creates the multithreaded server
/// @param argc amount of command line arguments
/// @param argv array of command line arguments
int main(int argc, char const *argv[])
{
    //initialize the data to be shared among the threads
    //items = parseData();

    //command line arguments
    shopAssitants = strtol(argv[1], NULL, 10);
    sofaAmount = strtol(argv[2], NULL, 10);
    waitingSpots = strtol(argv[3], NULL, 10);
    threadPoolSize = shopAssitants + sofaAmount + waitingSpots;

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
    for(int i = 0; i < 2; i++){
        pthread_create(&threadPool[i], &att, threadFuncShopAssitant, NULL);
    }

    //creates threads based on the amount of sofas
    for(int i = shopAssitants; i < shopAssitants + sofaAmount; i++){
        pthread_create(&threadPool[i], &att, threadFuncSofa, NULL);
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

        if(clientAmount >= threadPoolSize){
            //there is no space available in the shop, need to disconnect client.
            printf("%d > %d\n", clientAmount, threadPoolSize);
            disconnectClientDueToFullRoom(pclient);

        } else {
            //there is space available in the shop

            //beacause all threads are sharing the same queue structure, we have to sycnhronize enqueuing and dequeueing
            pthread_mutex_lock(&queueLock);

            clientAmount++;
            //prints the amount of clients in comparison to threadpool size (used for debugging and testing)
            printf("%d <= %d\n", clientAmount, threadPoolSize);
            //stores the client socket in the queue
            enqueue(pclient);

            //prints the rest of the queue(used for debugging and testing)
            printQ();
            
            
            if(clientAmount <= shopAssitants){
                //signals shop assitant that there is a new client
                pthread_cond_signal(&assitantConditionVar);
            }


            if(clientAmount > shopAssitants && clientAmount <= sofaAmount + shopAssitants){
                //client needs a sofa, so a signal is sent to a shop assitant thread
                pthread_cond_signal(&sofaConditionVar);
            }

            pthread_mutex_unlock(&queueLock);
        }
        

    }


    return 0;
}
