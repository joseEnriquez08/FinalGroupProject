//Author: Jose Enriquez
//Email: jose.enriquez@okstate.edu
//Date: 10/25/2022
//Description: TBD

#include "include.h"

/// @brief prints all data found in cartier_catalog.txt
/// @param items array of structs containing all the data
void printItems(struct item *items){
    for(int i = 0; i< items->length; i++){
        printf("Item %d: %s\t%s\t%s\t%s\t%s\t%s\t%s\t\n\n", i, items[i].ref, items[i].category,
            items[i].title , items[i].tags, items[i].description, items[i].quantity, items[i].price);
    
    }
}

/// @brief parses all the data in cartier_catalog.txt
/// @return pointer array of structs containing all the data
struct item* parseData(){

    char *fileName = "cartier_catalog.txt";
    FILE *file;
    file = fopen(fileName, "r");
    char buffer[300];

    if(file == NULL){
        printf("Unable to open file: %s\n", fileName);
        return NULL;
    }

    //decaltres appropriate sized array of structs
    int size = 46;
    struct item *items = calloc(size, sizeof(struct item));

    // keeps track of which column(title, genre...) we are on
    int column = 0;

    // keeps track of the rows. Used only to skip the first row which consists
    //      of the labels(title, genre...) for the columns
    int rows = 0;

    //index variable for accessing the books array
    int i = 0;

    //temp variable to help parse
    char *temp;
    char *temp2;

    //loops through each row
    while(fgets(buffer, sizeof(buffer), file) != NULL){
        column = 0;
        rows++;

        if(rows == 1){
            //skip first row, we dont need the columne labels
            continue;
        }

        if((strlen(buffer) > 0) && (buffer[strlen(buffer) - 1] == '\n')){
            //gets rid of the "\n" in each row
            buffer[strlen(buffer) - 1] = '\0';
        }

        
        //loops through each column in each row
        while(column < 7){

            if(column == 0){
                //parsing ref
                temp = strtok(buffer, "\t");
                strcpy(items[i].ref, temp);
                
                
            }

            if(column == 1){
                //parsing category
                temp = strtok(NULL, "\t");
                strcpy(items[i].category, temp);
                //printf("%s-----", temp);
                //temp = strtok(NULL, "\t");

            }
            
            if(column == 2){
                //pasrparsinging title
                temp = strtok(NULL, "\t");
                strcpy(items[i].title, temp);
                // if(temp[0] == '\"'){
                //     //temp = strtok(NULL, "\"");
                //     temp2 = temp;
                //     temp2++;
                //     temp2[strlen(temp2-1)] = '\0';
                //     printf("%s-----", temp2);
                // }
                
                //printf("%s-----", temp);
                
            }
            
            if(column == 3){
                //parsing tags
                temp = strtok(NULL, "\t");
                strcpy(items[i].tags, temp);
                //printf("%s-----", temp);
                

            }

            if(column == 4){
                //parsing description
                temp = strtok(NULL, "\t");
                strcpy(items[i].description, temp);
                //printf("%s-----", temp);
                //temp = strtok(NULL, "\t");

            }

            if(column == 5){
                //parsing quantity
                temp = strtok(NULL, "\t");
                strcpy(items[i].quantity, temp);
                //printf("%s-----", temp);
                //temp = strtok(NULL, "\t");

            }

            if(column == 6){
                //parsing price
                temp = strtok(NULL, "\t");
                strcpy(items[i].price, temp);
                //printf("%s", temp);
                

            }
            
            items[i].length = size;
            column++;
        }
        //printf("\n");
        i++;
    }
    fclose(file);

    // //trying to remove quotation marks
    // for(int i = 0; i< items->length; i++){
    //     if(items[i].title[0] == '\"'){
    //         memmove(items[i].title, items[i].title+1, strlen(items[i].title));
    //         items[i].title[strlen(items[i].title-1)] = 0;
    //         printf("%s\n", items[i].title);
    //     }
    // }

    //printItems(items);
    return items;
    
}

//shows how to use the function
// int main(int argc, char const *argv[])
// {
//     /* code */
//     struct item *items = parseData();
//     for(int i = 0; i< items->length; i++){
//         //do stuff
//     }
//     return 0;
// }
