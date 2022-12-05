//Author: Adil Aman Mohammed
//Email: adil.mohammed@okstate.edu
//Date: 12/01/2022
//Description: This file implements option 3 - purchase N number of items and print there details with total sum of purchase.



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "include.h"

//This functions implements option 3- Buying

void purchaseJewelfunc(int clientSocket, struct Catalog clientPurchases[]){
    char buffer[BUFSIZE] = {0};

    //asks client on how many items he/she wants to buy
    sprintf(buffer, "Please enter the amount of items you plan to buy\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");

    //stores clients input
    int numOfitems = (int) strtol(buffer, NULL, 10);
    buffer[0] = 0;

    //Store the client items(ref nums)
    struct Array itemsToPurchase[numOfitems];

    //client to enter the input ref one by one
    sprintf(buffer, "Please enter the specific ref values of the Items you wish to buy\n"
            "Enter one at a time please\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //get all the items one by one
    for(int i = 0; i < numOfitems; i++){
        //reads ref entered by client and stores it into itemsToPurchase array
        check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
        strcpy(itemsToPurchase[i].str, buffer);
        buffer[0] = 0;

        //setting the total amount
        itemsToPurchase[i].length = numOfitems;
    }


    pthread_mutex_lock(&catalogLock);

    //boolean variable to check if item has been found;
    bool isfound = false;

    //keep track of total amount
    int total = 0;

    //for each ref entered by the client, loop through inventory
    for(int i = 0; i < numOfitems; i++){
        isfound = false;
        for(int j = 0; j < 46; j++){

            if(strcmp(itemsToPurchase[i].str, catalogstr[j].ref) == 0 && catalogstr[j].quantity > 0){
                //item is found and in stock

                isfound = true;

                //increase quantity of same ref number in client
                clientPurchases[j].quantity ++;

                //decrease quantity of same ref number in inverntoty
                catalogstr[j].quantity--;

                //add to total amount
                total = total + catalogstr[j].price;

                //writes to client, the purchased item.
                sprintf(buffer, "Purchased Item: \nCategory: %s \nDescription: %s \nPrice: %d\n",
                    catalogstr[j].category, catalogstr[j].description, catalogstr[j].price);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
                break;
            }

            else if(strcmp(itemsToPurchase[i].str, catalogstr[j].ref) == 0 && catalogstr[j].quantity <= 0){
                //item is out of stock
                isfound = true;
                //writes to client telling the the item is out of stock
                sprintf(buffer, "Item: %s is out of stock. Please buy another item.\n",
                    catalogstr[j].ref);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
            }
        }

        if(!isfound){
            //item does not exist;
            //The clients given ref number does not exist purchase
            sprintf(buffer, "Item: \"%s\" does not exist.\n",
                itemsToPurchase[i].str);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
        }
    }
    pthread_mutex_unlock(&catalogLock);

    //Sending to client the total amount purchased
    sprintf(buffer, "Total: %d\n", total);
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
}



// int main(int argc, char *argv[]){

// 	if(readFile()==-1){
// 		printf("Error in Reading Cartier_catalog.txt\n");exit(1);
// 	}

//       char refnum[20];
//       strcpy(refnum, argv[1]);
//       char* returned = purchaseJewelfunc(refnum);
//       printf("The purchased message is %s \n",returned);
//       a =a+1;


//   return 0;

// }

