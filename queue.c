//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD

#include "include.h"

struct node *head = NULL;
struct node *tail = NULL;
int qSize = 0;

void enqueue(int *clientSocket){
    struct node *newNode = malloc(sizeof(struct node));
    newNode->clientSocket = clientSocket;
    newNode->next = NULL;
    newNode->beingHandledByShopAssistant = 0;
    newNode->beingHandledBySofa = 0;
    newNode->beingHandledByRoom = 0;
    newNode->index = qSize;
    if(tail == NULL){
        head = newNode;
    } else {
        tail->next = newNode;
    }

    tail= newNode;
    qSize++;
}

struct node* dequeue() {
    if(head == NULL){
        return NULL;
    } else {
        //int *result = head->clientSocket;
        struct node *temp = head;
        head = head->next;
        if(head == NULL) {
            tail = NULL;
        }

        struct node *temp2 = head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }

        qSize--;
        return temp;
        
        //return result;
    }
}

int removeNode(int index){
    struct node *temp = head;
    struct node *prev = NULL;

    if(temp != NULL && temp->index == index){
        head = head->next;

        struct node *temp2 = head;
        while(temp2 != NULL){
            temp2->index--;
            temp2 = temp2->next;
        }
        qSize--;
        return 0;
    }

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
    while(temp2 != NULL){
        temp2->index--;
        temp2 = temp2->next;
    }
    qSize--;
}

int getQueueSize(){
    return qSize;
}

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
