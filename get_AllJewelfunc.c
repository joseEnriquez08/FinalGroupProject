//Author:Ganesh Mahendra Prabhu
//Email:ganesh.prabhu@okstate.edu
//Date: 12/01/2022
//Description: This file gets and returns the data based on refnum



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "include.h"

void getAllJewelfunc(int clientSocket){
    char buffer[BUFSIZE] = {0};
    pthread_mutex_lock(&catalogLock);

    sprintf(buffer,"Ref\t\tCategory\t\tTitle\t\tTags\t\tPrice\n");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    //loops through the inventory
    for(int i = 0; i< 46; i++){
        //loads buffer with single item from catalog
        sprintf(buffer, "%s\t%s\t%s\t%s\t%d\n",catalogstr[i].ref,catalogstr[i].category,catalogstr[i].title,catalogstr[i].tags,catalogstr[i].price);

        //sends item to client;
        check(fullwrite(clientSocket, buffer, sizeof(buffer)), "Sending/Writing failed");
    }
    pthread_mutex_unlock(&catalogLock);
}

// int main(int argc, char *argv[]){

// 	if(readFile()==-1){
// 		printf("Error in Reading Cartier_catalog.txt\n");exit(1);
// 	}

//      getAllJewelfunc();

//  return 0;

// }

