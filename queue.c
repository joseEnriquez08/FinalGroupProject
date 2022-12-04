//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: Implements Queue like structure using linked a linked list. Helps in adding the first come first serve functionality

//Note may have to make queue structurse so we can make multiple queues instead of just one 
#include "include.h"


// struct node *head = NULL;
// struct node *tail = NULL;
// int qSize = 0;
//struct queue assitantQueue;


void enqueueFromSofa(int clientSocket, int sofaID, struct queue *q){
    //creates and initializes a new node
    struct node *newNode = malloc(sizeof(struct node));
    //int i = 0;
    newNode->clientSocket = clientSocket;
    //printf("%d\n", (newNode->clientSocket));
    newNode->next = NULL;
    newNode->beingHandledByShopAssistant = 0;
    newNode->beingHandledBySofa = 0;
    newNode->beingHandledByRoom = 0;
    newNode->sofaID = sofaID;
    newNode->waitID = 0;
    newNode->index = q->qSize;

    if(q->tail == NULL){
        //queue is empty
        q->head = newNode;
        if(assitantQueue.head == NULL){
            printf("Is null first\n");
        }
    } else {
        //queue is not empty
        q->tail->next = newNode;
    }

    //updates tail q size
    q->tail= newNode;
    q->qSize++;
    if(assitantQueue.tail == NULL){
        printf("Is null first\n");
    }
}


void enqueueFromWaiting(int clientSocket, int waitID, struct queue *q){
    //creates and initializes a new node
    struct node *newNode = malloc(sizeof(struct node));
    //int i = 0;
    newNode->clientSocket = clientSocket;
    //printf("%d\n", (newNode->clientSocket));
    newNode->next = NULL;
    newNode->beingHandledByShopAssistant = 0;
    newNode->beingHandledBySofa = 0;
    newNode->beingHandledByRoom = 0;
    newNode->waitID = waitID;
    newNode->index = q->qSize;
    newNode->sofaID = 0;

    if(q->tail == NULL){
        //queue is empty
        q->head = newNode;
        if(sofaQueue.head == NULL){
            printf("Is null first\n");
        }
    } else {
        //queue is not empty
        q->tail->next = newNode;

    }

    //updates tail q size
    q->tail= newNode;
    q->qSize++;
    if(sofaQueue.tail == NULL){
        printf("Is null first\n");
    }
}


/// @brief makes a client node and puts it at the end of the queue
/// @param clientSocket
void enqueue(int clientSocket, struct queue *q){
    //creates and initializes a new node
    struct node *newNode = malloc(sizeof(struct node));
    //int i = 0;
    newNode->clientSocket = clientSocket;
    //printf("%d\n", (newNode->clientSocket));
    newNode->next = NULL;
    newNode->beingHandledByShopAssistant = 0;
    newNode->beingHandledBySofa = 0;
    newNode->beingHandledByRoom = 0;
    newNode->sofaID = 0;
    newNode->waitID = 0;
    newNode->index = q->qSize;

    if(q->tail == NULL){
        //queue is empty
        q->head = newNode;
        if(assitantQueue.head == NULL){
            printf("Is null first\n");
        }
    } else {
        //queue is not empty
        q->tail->next = newNode;
    }

    //updates tail q size
    q->tail= newNode;
    q->qSize++;
    if(assitantQueue.tail == NULL){
        printf("Is null first\n");
    }
}

/// @brief removes node at the front of the queue
struct node* dequeue(struct queue *q) {
    if(q->head == NULL){
        //queue is empty 
        return NULL;
    } else {
        //queue is not empty

        //node to return
        struct node *temp = q->head;
        //updates head and disconncets old head;
        q->head = q->head->next;
        if(q->head == NULL) {
            //queue only had one node. need to update tail
            q->tail = NULL;
        }

        //case: queue had more than one node and need to update the index variable(waiting spot) in each node
        //this makes it different than the traditional queue, since dequeue is now O(N);
        struct node *temp2 = q->head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }

        q->qSize--;
        return temp;
    }
}


/// @brief removes node from specific index
/// @param index of node to be removed
/// @return
int removeNode(int index, struct queue *q){
    struct node *temp = q->head;
    struct node *prev = NULL;

    if(temp != NULL && temp->index == index){
        //index was 0, removing head
        q->head = q->head->next;

        //update index variable of the rest of the heads
        struct node *temp2 = q->head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }
        q->qSize--;
        return 0;
    }


    //loop to find the specific node
    while(temp != NULL && temp->index != index){
        prev = temp;
        temp = temp->next;
    }

     // If key was not present in linked list
    if (temp == NULL){
        return -1;
    }
    
  
    // Unlink the node from linked list
    prev->next = temp->next;
    struct node *temp2 = prev->next;

    //update index variable of the rest of the heads
    while(temp2 != NULL){
        temp2->index--;
        temp2 = temp2->next;
    }
    q->qSize--;
}

/// @return size of queue 
int getQueueSize(struct queue *q){
    return q->qSize;
}

/// @brief prints the queue
void printQ(struct queue *q){
    struct node *temp = q->head;
    while(temp != NULL){
        //printf("hi");
        //struct node *temp = dequeue();
        printf("(%d, %d)->", (temp->clientSocket), temp->index);
        temp = temp->next;
    }
    printf("\n");
}

/// @brief get the next node not being handled by a sofa
struct node* getNextNonHandleBySofa(struct queue *q){
    if(q->head == NULL){
        return NULL;
    }

    struct node *temp = q->head;
    while(temp != NULL){
        if(temp->beingHandledBySofa == 0){
            return temp;
        }
        temp = temp->next;
    }
}

/// @brief retursns head of queue without removing
struct node* peek(struct queue *q){
    return q->head;
}

// int main(int argc, char const *argv[])
// {

    
//     assitantQueue.head = NULL;
//     assitantQueue.tail = NULL;
//     struct queue sofaQueue;
//     struct queue waitingRoomQueue;

//     assitantQueue.qSize = 0;
//     sofaQueue.qSize = 0;
//     waitingRoomQueue.qSize = 0;

//     int i1 = 1;
//     int i2 = 2;
//     int i3 = 3;
//     int i4 = 4;
//     int i5 = 5;

//     enqueue(i1, &assitantQueue);
//     printQ(&assitantQueue);
//     if(assitantQueue.head == NULL){
//         printf("Is null\n");
//     }
//     // enqueue(&i2);
//     // enqueue(&i3);
//     // enqueue(&i4);
//     // enqueue(&i5);

   
// }
