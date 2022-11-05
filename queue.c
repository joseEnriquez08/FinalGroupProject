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
    if(tail == NULL){
        head = newNode;
    } else {
        tail->next = newNode;
    }

    tail= newNode;
    qSize++;
}

int* dequeue() {
    if(head == NULL){
        return NULL;
    } else {
        int *result = head->clientSocket;
        struct node *temp = head;
        head = head->next;
        if(head == NULL) {
            tail = NULL;
        }
        free(temp);
        qSize--;
        return result;
    }
}

int getQueueSize(){
    return qSize;
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
//     while(temp != NULL){
//         //printf("hi");
//         printf("%d->", *(temp->clientSocket));
//         temp = temp->next;
//     }

//     printf("\n");
//     int i = 0;

//     while(i<5){
//         //printf("hi");
//         printf("%d->", *dequeue());
//         i++;
//     }
//     return 0;
// }
