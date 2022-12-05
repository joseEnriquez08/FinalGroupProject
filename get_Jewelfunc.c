// Author: Venkata Ragavendra Vavilthota
// Email: venkat_ragav.vavilthota@okstate.edu
// Date: 12/01/2022
// Description: This file gets and returns the data based on refnum

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "include.h"

void getJewelfunc(int clientSocket) {
  char buffer[BUFSIZE] = {0};

  // boolean variable to check if item has been found;
  bool isfound = false;

  // writes to client asking for specific ref and reads their input
  sprintf(buffer, "Please enter the specific ref\n");
  check(fullwrite(clientSocket, buffer, sizeof(buffer)),
        "Sending/Writing failed");
  buffer[0] = 0;
  check(fullread(clientSocket, buffer, sizeof(buffer)), "Reading failed");

  pthread_mutex_lock(&catalogLock);
  // loops through inventory to find matching ref
  for (int i = 0; i < 46; i++) {
    if (strcmp(catalogstr[i].ref, buffer) == 0) {
      // found matching ref, item exists
      // sends client info about item
      sprintf(buffer,
              "Ref:\t %12s\n"
              "Category: %11s\n"
              "Title:\t %17s\n"
              "Tags:\t %15s\n"
              "Description: %s\n"
              "Price:\t %9d\n"
              "Quantity: %4d\n",
              catalogstr[i].ref, catalogstr[i].category, catalogstr[i].title,
              catalogstr[i].tags, catalogstr[i].description,
              catalogstr[i].price, catalogstr[i].quantity);
      check(fullwrite(clientSocket, buffer, sizeof(buffer)),
            "Sending/Writing failed");

      // changes boolean variable
      isfound = true;
      break;
    }
  }
  pthread_mutex_unlock(&catalogLock);

  if (!isfound) {
    // ref was not found. Notifying client
    sprintf(buffer, "Invalid reference number");
    check(fullwrite(clientSocket, buffer, sizeof(buffer)),
          "Sending/Writing failed");
  }
  
  
  return;
}

// int main(int argc, char *argv[]){

// 	if(readFile()==-1){
// 		printf("Error in Reading Cartier_catalog.txt\n");exit(1);
// 	}

//       char refnum[20];
//       strcpy(refnum, argv[1]);
//       struct Catalog CatalogStruct =  getJewelfunc(refnum);
//             printf("Ref\tCategory\tTitle\tTags\tDescription\tPrice\n%s\t%s\t%s\t%s\t%s\t%d",CatalogStruct.ref,CatalogStruct.category,CatalogStruct.title,CatalogStruct.tags,CatalogStruct.description,CatalogStruct.price);

//   return 0;

// }
