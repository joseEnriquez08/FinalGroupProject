//Author: Venkata Ragavendra Vavilthota
//Email: venkat_ragav.vavilthota@okstate.edu
//Date: 12/01/2022
//Description: This file keeps all the parsed data in the struct

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

extern struct Catalog catalogstr[46];	//array of catalog

int readFile();

#endif
