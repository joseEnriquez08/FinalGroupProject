//Author: Venkata Ragavendra Vavilthota
//Email: venkat_ragav.vavilthota@okstate.edu
//Date: 11/15/2022
//Description: This file gets and returns the data based on refnum



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "include.h"


void purchaseJewelfunc(int clientSocket, struct Catalog clientPurchases[]){
    char buffer[BUFSIZE] = {0};

    //writes to client asking how many items they want to purchase and reads their input
    sprintf(buffer, "Please enter the amount of items you plan to buy\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");

    //stores clients input of how many items they will purchase
    int numOfitems = (int) strtol(buffer, NULL, 10);
    buffer[0] = 0;

    //Array to store the refs the client is attempting to purchase.
    struct Array itemsToPurchase[numOfitems];

    //writes to client asking to enter the ref values
    sprintf(buffer, "Please enter the specific ref values of the Items you wish to buy\n"
            "Enter one at a time please\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //loop to read all the refs entered by the client
    for(int i = 0; i < numOfitems; i++){
        //reads ref entered by client and stores it into itemsToPurchase array
        check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
        strcpy(itemsToPurchase[i].str, buffer);
        buffer[0] = 0;

        //sets length memeber of each struct in the array
        itemsToPurchase[i].length = numOfitems;
    }


//     pthread_mutex_lock(&catalogLock);

    //boolean variable to check if item has been found;
    int found = 0;

    //sum to keep track of the total amount purchased
    int sum = 0;

    //for each ref entered by the client, loopp through inventory to check if its valid
    for(int i = 0; i < itemsToPurchase->length; i++){
        found = 0;
        for(int j = 0; j < 46; j++){

            if(strcmp(itemsToPurchase[i].str, catalogstr[j].ref) == 0 && catalogstr[j].quantity > 0){
                //item is found and in stock

                found = 1;

                //increment quantity of item into clientPurchases to keep track of purchases for returning.
                clientPurchases[j].quantity ++;

                //decrement quantity of item in storewide inventory: items array.
                catalogstr[j].quantity--;

                //add to some new purchase price
                sum = sum + catalogstr[j].price;

                //writes to client, the purchased item.
                sprintf(buffer, "Purchased Item: Category: %s Description: %s Price: %d\n",
                    catalogstr[j].category, catalogstr[j].description, catalogstr[j].price);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
                break;
            }

            else if(strcmp(itemsToPurchase[i].str, catalogstr[j].ref) == 0 && catalogstr[j].quantity <= 0){
                //item is found but out of stock
                found = 1;
                //writes to client telling the the item is out of stock
                sprintf(buffer, "Item: \"%s\" out of stock.\n",
                    catalogstr[j].ref);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
            }
        }

        if(found == 0){
            //item does not exist;
            //writes to client telling them the item they tried to purchase does not exist
            sprintf(buffer, "Item: \"%s\" does not exist.\n",
                itemsToPurchase[i].str);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
        }
    }
//     pthread_mutex_unlock(&catalogLock);

    //writes to client telling them of their total amount they spent on the purchase
    sprintf(buffer, "Total: %d\n", sum);
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

