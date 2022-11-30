//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 11/18/2022
//Description: This file implements the first 4 menu funtions that the client will use.

#include "include.h"

//to make these functions thread safe, i have synchronized the access of the items array that contains
//all the data from the cartier_catalog.txt. This mutex will allow only one thread/shop assitant to access
//the items
pthread_mutex_t itemsLock = PTHREAD_MUTEX_INITIALIZER;

/// @brief Implements menu option 1. Send the entire inventory to the client;
/// @param clientSocket used to communicate with the client
void getAllItems(int clientSocket){
    char buffer[BUFSIZE] = {0};
    pthread_mutex_lock(&itemsLock);

    //loops through the inventory
    for(int i = 0; i< items->length; i++){
        //loads buffer with single item from catalog 
        sprintf(buffer, "Item %d: %s\t%s\t%s\t%s\t%s\n\n", i, items[i].ref, items[i].category, 
            items[i].title, items[i].tags, items[i].price);//, );//%s\t%s\t%s\t%s\t%s\t%s\t\n", i, items[i].ref, items[i].category,
        
        //sends item to client;
        check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    pthread_mutex_unlock(&itemsLock);
}

/// @brief Implements option 2: Sends client information about a specific item based on ref
/// @param clientSocket used to communicate with the client
void getItemInfo(int clientSocket){
    char buffer[BUFSIZE] = {0};
    
    //boolean variable to check if item has been found;
    int found = 0;

    //writes to client asking for specific ref and reads their input
    sprintf(buffer, "Please enter the specific ref\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");

    pthread_mutex_lock(&itemsLock);
    //loops through inventory to find matching ref
    for(int i = 0; i < items->length; i++){
        
        if(strcmp(items[i].ref, buffer) == 0){
            //found matching ref, item exists
            //sends client info about item
            sprintf(buffer, "Ref:\t %12s\n" "Category: %11s\n" "Title:\t %17s\n"
                    "Tags:\t %15s\n" "Description: %s\n" "Price:\t %9s\n"
                    "Quantity: %4s\n", items[i].ref, items[i].category, items[i].title,
                    items[i].tags, items[i].description, items[i].price, items[i].quantity);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            
            //changes boolean variable
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&itemsLock);

    if(found == 0){
        //ref was not found. Notifying client
        sprintf(buffer, "Invalid ref");
        check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    return;
}

/// @brief Implements option 3. Asks client what items they want to purchasse based on ref.
/// @param clientSocket used to communicate with the client
/// @param clientPurchases items array used to keep track of the clients purchases/returns
void makePurchase(int clientSocket, struct item clientPurchases[]){
    char buffer[BUFSIZE] = {0};

    //writes to client asking how many items they want to purchase and reads their input
    sprintf(buffer, "Please enter the amount of items you plan to buy\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
    check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
    
    //stores clients input of how many items they will purchase
    int purchaseAmount = (int) strtol(buffer, NULL, 10);
    buffer[0] = 0;

    //Array to store the refs the client is attempting to purchase.
    struct Array itemsToPurchase[purchaseAmount];

    //writes to client asking to enter the ref values
    sprintf(buffer, "Please enter the specific ref values of the Items you wish to buy\n"
            "Enter one at a time please\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;

    //loop to read all the refs entered by the client
    for(int i = 0; i < purchaseAmount; i++){
        //reads ref entered by client and stores it into itemsToPurchase array
        check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");
        strcpy(itemsToPurchase[i].str, buffer);
        buffer[0] = 0;
        
        //sets length memeber of each struct in the array
        itemsToPurchase[i].length = purchaseAmount;
    }

    pthread_mutex_lock(&itemsLock);

    //boolean variable to check if item has been found;
    int found = 0;

    //sum to keep track of the total amount purchased
    int sum = 0;

    //for each ref entered by the client, loopp through inventory to check if its valid
    for(int i = 0; i < itemsToPurchase->length; i++){
        found = 0;
        for(int j = 0; j < items->length; j++){

            if(strcmp(itemsToPurchase[i].str, items[j].ref) == 0 &&
                (int) strtol(items[j].quantity, NULL, 10) > 0){
                //item is found and in stock

                found = 1;

                //increment quantity of item into clientPurchases to keep track of purchases for returning.
                int newClientQuantity = (strtol(clientPurchases[j].quantity, NULL, 10)) + 1;
                sprintf(clientPurchases[j].quantity, "%d", newClientQuantity);
                
                //decrement quantity of item in storewide inventory: items array.
                int newQuantity = (strtol(items[j].quantity, NULL, 10)) - 1;
                sprintf(items[j].quantity, "%d", newQuantity);

                //add to some new purchase price
                sum += (int) strtol(items[j].price, NULL, 10);

                //writes to client, the purchased item.
                sprintf(buffer, "Purchased Item: Category: %s Description: %s Price: %s\n",
                    items[j].category, items[j].description, items[j].price);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
                break;
            }

            else if(strcmp(itemsToPurchase[i].str, items[j].ref) == 0 &&
                strtol(items[j].quantity, NULL, 10) <= 0){
                //item is found but out of stock
    
                found = 1;
                //writes to client telling the the item is out of stock
                sprintf(buffer, "Invalid purchase. Item: \"%s\" out of stock.\n",
                    itemsToPurchase[i].str);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
            }
        }

        if(found == 0){
            //item does not exist;
            //writes to client telling them the item they tried to purchase does not exist
            sprintf(buffer, "Invalid purchase. Item: \"%s\" does not exist.\n",
                itemsToPurchase[i].str);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
        }
        
    }
    pthread_mutex_unlock(&itemsLock);

    //writes to client telling them of their total amount they spent on the purchase
    sprintf(buffer, "Total: %d\n", sum);
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    buffer[0] = 0;
}

/// @brief Implements option 4. Asks client what items they want to return based on ref.
/// @param clientSocket used to communicate with the client
/// @param clientPurchases 
void returnItem(int clientSocket, struct item clientPurchases[]){
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

    pthread_mutex_lock(&itemsLock);
    //boolean variable to check if item ahs been found
    int found = 0;

    //for each ref entered by the client, loopp through inventory to check if its valid
    for(int i = 0; i < itemsToReturn->length; i++){
        found = 0;
        for(int j = 0; j < items->length; j++){

            if(strcmp(itemsToReturn[i].str, items[j].ref) == 0 &&
                (int) strtol(clientPurchases[j].quantity, NULL, 10) > 0){
                //client has purchased it before. and item has been found. valid to return.

                found = 1;

                //decrement item quantity from clientPurchases to keep track of purchases and returns.
                int newClientQuantity = (strtol(clientPurchases[j].quantity, NULL, 10)) - 1;
                sprintf(clientPurchases[j].quantity, "%d", newClientQuantity);
                buffer[0] = 0;

                //increment item quanitity in storewide inveventory: items
                int newQuantity = (strtol(items[j].quantity, NULL, 10)) + 1;
                sprintf(items[j].quantity, "%d", newQuantity);
            
                //write to client telling them of the item they purchase
                sprintf(buffer, "Returned Item: Category: %s Description: %s Price: %s\n",
                    clientPurchases[j].category, clientPurchases[j].description, clientPurchases[j].price);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
                break;
            }

            else if(strcmp(itemsToReturn[i].str, items[j].ref) == 0 &&
                (int) strtol(clientPurchases[j].quantity, NULL, 10) == 0){
                //client has not purchased this before, so they cant return it
                
                found = 1;
                
                //writes to client telling them they have not purchase this before
                sprintf(buffer, "Invalid return. Item: \"%s\" has not been purchased by you\n",
                itemsToReturn[i].str);
                check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
                buffer[0] = 0;
            }

        }

        if(found == 0){
            //does not exist;

            //writes to client telling them the item they tried to return does not exist
            sprintf(buffer, "Invalid return. Item: \"%s\" does not exist\n",
                itemsToReturn[i].str);
            check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
            buffer[0] = 0;
        }
        
    }
    pthread_mutex_unlock(&itemsLock);
}