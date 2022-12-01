//Author: Venkata Ragavendra Vavilthota
//Email: venkat_ragav.vavilthota@okstate.edu
//Date: 11/15/2022
//Description: This file gets and returns the data based on refnum



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "readFile.h"


char *purchaseJewelfunc(char refnum[]){

    char *pur = malloc(1000* sizeof(char));
  	for(int i=0;i<47;i++){
        if(strcmp(refnum,catalogstr[i].ref)==0){


          sprintf(pur, "\nThis item %s has been bought and deducted in list \nAmount Purchased:%d\n",catalogstr[i].category,catalogstr[i].price);

          catalogstr[i].quantity=catalogstr[i].quantity-1;
//          printf("The count in func is %d\n",catalogstr[i].quantity);

        }
    }
  return pur;
}


// int main(int argc, char *argv[]){

// 	if(readFile()==-1){
// 		printf("Error in Reading Cartier_catalog.txt\n");exit(1);
// 	}

//       char refnum[20];
//       strcpy(refnum, argv[1]);
//       char* returned = purchaseJewelfunc(refnum);
//       printf("The purchased message is %s \n",returned);

//   return 0;

// }

