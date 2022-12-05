//Author: Bhanu Teja Solipeta 
//Email: bhanu.solipeta@okstate.edu
//Date: 12/01/2022
//Description: This file gets teh refnum and returns the items to the store



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "include.h"


void returnJewelFunc(int clientSocket, struct Catalog clientPurchases[]){
    char buffer[BUFSIZE] = {0};

    //writes to client asking how many items they want to return and reads their input
    sprintf(buffer, "Please enter the amount of items you plan to return\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");

    //stores clients input of how many items they want to return
    int returnAmount = (int) strtol(buffer, NULL, 10);
    buffer[0] = 0;


    //array to store the refs the client will attempt to return
    struct Array itemsToReturn[returnAmount];

    //writes to client asking to enter the ref values
    sprintf(buffer, "Please enter the specific ref values of the Items you wish to return\n"
            "Enter one at a time please\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //loop to read all the refs entered by the client
    for(int i = 0; i < returnAmount; i++){
        ///reads ref entered by client and stores it into itemsToReturn array 
        check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
        strcpy(itemsToReturn[i].str, buffer);
        buffer[0] = 0;
        itemsToReturn[i].length = returnAmount;
    }

   pthread_mutex_lock(&catalogLock);
    //boolean variable to check if item ahs been found
    bool isfound = false;

    //for each ref entered by the client, loopp through inventory to check if its valid
    for(int i = 0; i < itemsToReturn->length; i++){
        isfound = false;
        for(int j = 0; j < 46; j++){

            if(strcmp(itemsToReturn[i].str, catalogstr[j].ref) == 0 && clientPurchases[j].quantity > 0){
                //client has purchased it before. and item has been found. valid to return.

                isfound = true;

                //decrement item quantity from clientPurchases to keep track of purchases and returns.
                clientPurchases[j].quantity--;

                //increment item quanitity in storewide inveventory: items
                catalogstr[j].quantity++;

                //write to client telling them of the item they purchase
                sprintf(buffer, "Returned Item: Category: %s Description: %s Price: %d\n",
                    clientPurchases[j].category, clientPurchases[j].description, clientPurchases[j].price);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
                break;
            }

            else if(strcmp(itemsToReturn[i].str, catalogstr[j].ref) == 0 &&
                clientPurchases[j].quantity == 0){
                //client has not purchased this before, so they cant return it

                isfound = true;

                //writes to client telling them they have not purchase this before
                sprintf(buffer, "Invalid return. Item: \"%s\" has not been purchased by you\n",
                itemsToReturn[i].str);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
            }

        }

        if(!isfound){
            //does not exist;

            //writes to client telling them the item they tried to return does not exist
            sprintf(buffer, "Invalid return. Item: \"%s\" does not exist\n",
                itemsToReturn[i].str);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
        }

    }
   pthread_mutex_unlock(&catalogLock);
}



// int main(int argc, char *argv[]){

// 	if(readFile()==-1){
// 		printf("Error in Reading Cartier_catalog.txt\n");exit(1);
// 	}

//       char refnum[20];
//       strcpy(refnum, argv[1]);
//       char* returned = returnJewelfunc(refnum);
//       printf("The returned message is %s \n",returned);

//   return 0;

// }

