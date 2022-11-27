//Author: Venkata Ragavendra Vavilthota
//Email: venkat_ragav.vavilthota@okstate.edu
//Date: 11/12/2022
//Description: This file reads the data from txt file and puts it in catalog struct 

#include "readFile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int readFile(){
//	FILE *infile;
	int res=-1;
	int itr=0;

    FILE *in = fopen("cartier_catalog.txt", "r");

    if (in != NULL)
    {
        char line[1024];

        while (fgets(line, sizeof line, in) != NULL)
        {
            char field[100];
            int offset = 0;
            int columnitr=0;

            // Break down the line based on tabs ('\n' included because fgets retains a newline if it's present)
            while (sscanf(line + offset, "%[^\t\n]", field) == 1)
            {
               // puts(field);

                switch(columnitr){
					case 0:
						memcpy(catalogstr[itr].ref,field,sizeof(field));
						break;
					case 1:
						memcpy(catalogstr[itr].category,field,sizeof(field));
						break;
					case 2:
						memcpy(catalogstr[itr].title,field,sizeof(field));
						break;
					case 3:
						memcpy(catalogstr[itr].tags,field,sizeof(field));
						break;
					case 4:
						memcpy(catalogstr[itr].description,field,sizeof(field));
						break;
					case 5:
						catalogstr[itr].quantity=atoi(field);
						break;
					case 6:
						catalogstr[itr].price=atoi(field);
						break;

                }

                offset += strlen(field);

                // Safety check to avoid stepping off the end of the array
                if (line[offset] != '\0')
                {
                    ++offset;
                }
                columnitr++;
            }
            //printf("ref:%s\tcat:%s\ttitle:%s\ttags:%s\tdescription:%squantity:%d\tprice:%d\n",catalogstr[itr].ref,catalogstr[itr].category,catalogstr[itr].title,catalogstr[itr].tags,catalogstr[itr].description,catalogstr[itr].quantity,catalogstr[itr].price);
            itr++;
        }
        res=0;
        fclose(in);
    }

	return res;
}
