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


struct Catalog getJewelfunc(char refnum[]){
//     printf("refnum %s \n",refnum);

    struct Catalog CatalogStruct;
  	for(int i=0;i<47;i++){
        if(strcmp(refnum,catalogstr[i].ref)==0){
              CatalogStruct=  catalogstr[i];

        }
    }

  return CatalogStruct;
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

