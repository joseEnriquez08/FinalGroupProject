//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: Implements Queue like structure using linked a linked list. Helps in adding the first come first serve functionality

//Note may have to make queue structurse so we can make multiple queues instead of just one 
#include "include.h"


struct node *head = NULL;
struct node *tail = NULL;
int qSize = 0;

/// @brief makes a client node and puts it at the end of the queue
/// @param clientSocket 
void enqueue(int *clientSocket){
    //creates and initializes a new node
    struct node *newNode = malloc(sizeof(struct node));
    newNode->clientSocket = clientSocket;
    newNode->next = NULL;
    newNode->beingHandledByShopAssistant = 0;
    newNode->beingHandledBySofa = 0;
    newNode->beingHandledByRoom = 0;
    newNode->index = qSize;

    if(tail == NULL){
        //queue is empty
        head = newNode;
    } else {
        //queue is not empty
        tail->next = newNode;
    }

    //updates tail q size
    tail= newNode;
    qSize++;
}

/// @brief removes node at the front of the queue
struct node* dequeue() {
    if(head == NULL){
        //queue is empty 
        return NULL;
    } else {
        //queue is not empty

        //node to return
        struct node *temp = head;
        //updates head and disconncets old head;
        head = head->next;
        if(head == NULL) {
            //queue only had one node. need to update tail
            tail = NULL;
        }

        //case: queue had more than one node and need to update the index variable(waiting spot) in each node
        //this makes it different than the traditional queue, since dequeue is now O(N);
        struct node *temp2 = head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }

        qSize--;
        return temp;
    }
}


/// @brief removes node from specific index
/// @param index of node to be removed
/// @return 
int removeNode(int index){
    struct node *temp = head;
    struct node *prev = NULL;

    if(temp != NULL && temp->index == index){
        //index was 0, removing head
        head = head->next;

        //update index variable of the rest of the heads
        struct node *temp2 = head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }
        qSize--;
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
    qSize--;
}

/// @return size of queue 
int getQueueSize(){
    return qSize;
}

/// @brief prints the queue
void printQ(){
    struct node *temp = head;
    while(temp != NULL){
        //printf("hi");
        //struct node *temp = dequeue();
        printf("(%d, %d)->", *(temp->clientSocket), temp->index);
        temp = temp->next;
    }
    printf("\n");
}

/// @brief get the next node not being handled by a sofa
struct node* getNextNonHandleBySofa(){
    if(head == NULL){
        return NULL;
    }

    struct node *temp = head;
    while(temp != NULL){
        if(temp->beingHandledBySofa == 0){
            return temp;
        }
        temp = temp->next;
    }
}

/// @brief retursns head of queue without removing
struct node* peek(){
    return head;
}

// int main(int argc, char const *argv[])
// {
//     int i1 = 1;
//     int i2 = 2;
//     int i3 = 3;
//     int i4 = 4;
//     int i5 = 5;

//     enqueue(&i1);
//     enqueue(&i2);
//     enqueue(&i3);
//     enqueue(&i4);
//     enqueue(&i5);

//     struct node *temp = head;
//     
//     temp = head;
//     while(temp != NULL){
//         //printf("hi");
//         //struct node *temp = dequeue();
//         //printf("(%d, %d)->", *(temp->clientSocket), temp->index);
//         if(temp->index == 2) break;
//         temp->beingHandledBySofa = 1;
//         temp = temp->next;
//     }
    
//     printf("(%d, %d, %d)\n", *(temp->clientSocket), temp->index, temp->beingHandledBySofa);
//     temp = getNextNonHandleBySofa();
//     printf("(%d, %d, %d)\n", *(temp->clientSocket), temp->index, temp->beingHandledBySofa);
//     temp = peek();
//     printf("(%d, %d, %d)\n", *(temp->clientSocket), temp->index, temp->beingHandledBySofa);
    
//     //printf("\n");
//     temp = head;
//     while(temp != NULL){
//         //printf("hi");
//         //struct node *temp = dequeue();
//         printf("(%d, %d)->", *(temp->clientSocket), temp->index);
//         temp = temp->next;
//     }
//     printf("\n");
//     int j = qSize;

//     printf("Print q 1:");
//     printQ();
//     removeNode(2);
//     printf("Print q 2:");
//     printQ();

//     printf("Print q 3:");

// 
//     return 0;
// }
