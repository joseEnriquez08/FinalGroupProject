//Author: Venkata Ragavendra Vavilthota
//Email: venkat_ragav.vavilthota@okstate.edu
//Date: 11/12/2022
//Description: This .h file of readFile


#ifndef READFILE_H_
#define READFILE_H_
struct Catalog{
	char ref[20];
	char category[20];
	char title[30];
	char tags[50];
	char description[100];
	int quantity;
	int price;
};

struct Catalog catalogstr[47];	//array of catalog

int readFile();

#endif
