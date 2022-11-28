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
        sprintf(buffer, "Item %d: %s\t%s\t%s\t%s\t%s\n\n", i, items[i].ref, items[i].category, 
            items[i].title, items[i].tags, items[i].price);//, );//%s\t%s\t%s\t%s\t%s\t%s\t\n", i, items[i].ref, items[i].category,
            //items[i].title , items[i].tags, items[i].description, items[i].quantity, items[i].price);
        //printf("sending: %s", buffer);
        check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    pthread_mutex_unlock(&itemsLock);
    //fflush(stdout);

}

void getItemInfo(int clientSocket){
    char buffer[BUFSIZE] = {0};
    int found = 0;

    sprintf(buffer, "Please enter the specific ref\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
    printf("%s\n", buffer);

    pthread_mutex_lock(&itemsLock);
    for(int i = 0; i < items->length; i++){
        //printf("%s == %s\n", buffer, items->ref);

        if(strcmp(items[i].ref, buffer) == 0){
            sprintf(buffer, "Ref:\t %12s\n" "Category: %11s\n" "Title:\t %17s\n"
                    "Tags:\t %15s\n" "Description: %s\n" "Price:\t %9s\n"
                    "Quantity: %4s\n", items[i].ref, items[i].category, items[i].title,
                    items[i].tags, items[i].description, items[i].price, items[i].quantity);

            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&itemsLock);
    if(found == 0){
        sprintf(buffer, "Invalid ref");
        check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    return;
}

void makePurchase(){

}

void returnItem(){

}