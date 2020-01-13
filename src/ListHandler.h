/*
 * ListHandler.h
 *
 *  Created on: Jan 2, 2020
 *      Author: kjpra
 */

#ifndef LISTHANDLER_H_
#define LISTHANDLER_H_
#define MAX_LINE_LENGTH 256

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "LinkedList.h"

void printLine();
void createBackUp(char* filename);
void getInp(char* input, char* message);
int* findDate(char input[]);




void printLine(){
    printf("__________________________________________________________________________________________________\n");
}


void getInp(char* input, char* message){
    printf("%s", message);
    gets(input);
}

void createBackUp(char* filename){
	char tempFile[strlen(filename) + 5];
	strncpy(tempFile, filename, strlen(filename)-4);
	tempFile[strlen(filename)-4] = '\0'; //Debugging strncpy because for some reason it adds an extra "Rb@"
	strcat(tempFile, "OLD.txt");

	FILE *file, *write;
	file = fopen(filename, "r");
	write = fopen(tempFile, "w");

	char read[MAX_LINE_LENGTH];
	while(fgets(read, MAX_LINE_LENGTH, file) != NULL){
		fputs(read, write);
	}

	fclose(file);
	fclose(write);
}

//Returns an int array of size 3: {day, month, year}
int* findDate(char input[]){


	//Initialize an int array of 3 with all 0's.
	int* date = (int*) malloc(3*sizeof(int));
	for(int i =0; i < 3; i++) *(date+i) = 0;
    if(input[0] == 'u' || input[0] == 'U'){
        return date;
    }

	//Counter for pointer arithmetic
	int count = 0;

	for(int i = 0; (i < strlen(input) && count < 3); i++){
		if(input[i] == '/'){
			count++;
			continue;
		} else if (!isdigit(input[i])){
			if(input[i] == '\n') continue;
			printf("\nSomething went wrong with reading the date...\n");
			return (int*)0;
		}
		*(date+count) *= 10;
		*(date+count) += input[i] - '0';
	}

	//Flip the second and first entry because it is written as mm/dd/yy but needs to be stored d,m,y
	int temp = *(date+1);
	*(date+1) = *date;
	*date = temp;

	//In case 4 digit year is given
	*(date+2) %= 100;
    return date;

}


#endif /* LISTHANDLER_H_ */
