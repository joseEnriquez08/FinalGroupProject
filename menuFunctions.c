//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 11/18/2022
//Description: TBD

#include "include.h"

pthread_mutex_t itemsLock = PTHREAD_MUTEX_INITIALIZER;

void getAllItems(int clientSocket){

    char buffer[BUFSIZE] = {0};
    pthread_mutex_lock(&itemsLock);

    for(int i = 0; i< items->length; i++){
        sprintf(buffer, "Item %d: %s\t%s\t%s\t%s\t%s\t%s\t%s\n", i, items[i].ref, items[i].title, 
            items[i].title, items[i].tags, items[i].description, items[i].quantity, items[i].price);//, );//%s\t%s\t%s\t%s\t%s\t%s\t\n", i, items[i].ref, items[i].category,
            //items[i].title , items[i].tags, items[i].description, items[i].quantity, items[i].price);
        printf("sending: %s", buffer);
        check(write(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    pthread_mutex_unlock(&itemsLock);
    //fflush(stdout);

}

void getItemInfo(){

}

void makePurchase(){

}

void returnItem(){

}