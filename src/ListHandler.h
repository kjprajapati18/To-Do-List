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
void printList(char* filename);
void getInp(char* input, char* message);
void addEntry(char* filename);
void writeEntry(char* date, char* title, char* time, char* description, char* filename);
int* findDate(char input[]);




void printLine(){
    printf("\t__________________________________________________________________________________________________\n");
}

void printList(char* filename){
    char line[MAX_LINE_LENGTH];
    FILE* list = fopen(filename, "r");
    //Header in Console
    printf("\n\t\t\t\t\t\tTO-DO LIST::\n");
    printLine();

    //Actual Read/Print from file
    while(fgets(line, MAX_LINE_LENGTH, list) != NULL){
        printf(line);
    }

    printf("\n\n");
    fclose(list);
}

void getInp(char* input, char* message){
    printf("%s", message);
    gets(input);
}

void addEntry(char* filename){
	//Initialize variables needed
    char date[MAX_LINE_LENGTH];
    char title[MAX_LINE_LENGTH];
    char timeDue[20];
    char description[MAX_LINE_LENGTH];
    date[0] = '\0';
    title[0] = '\0';
    timeDue[0] = '\0';
    description[0] = '\0';

    //Request inputs
    getInp(date, "What day would you like to add an entry for (enter U for unordered list)? ");
    //Find which day to put this entry
    int day = -1;
    getInp(title, "Please enter a title: ");
    getInp(timeDue, "Enter a time due (Optional): ");
    getInp(description, "Enter a description (Optional): ");

    //-1 means that its a part of the unordered list*
    if(day == -1){
    	writeEntry("U", title, timeDue, description, filename);
    }


}

int* findDate(char input[]){
    if(input[0] == 'u' || input[0] == 'U'){
        return NULL;
    }

	int* date = (int*) malloc(3*sizeof(int));
	for(int i =0; i < 3; i++) *(date+i) = 0;
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

	//Flip the second and first entry because it is written as mm/dd/yyyy but needs to be stored d,m,y
	int temp = *(date+1);
	*(date+1) = *date;
	*date = temp;

    return date;

}

void writeEntry(char* date, char* title, char* time, char* description, char* filename){

	FILE* list = fopen(filename, "r+");
	int spacing = 9;
	//Calculate the amount of space needed between title and time to keep things lined up.
	char dtTabs[spacing+1];
	int numDT = spacing - ((strlen(title)-1)/8);
	if (numDT < 0) numDT = 0;
	for(int i = 0; i < numDT; i++) dtTabs[i] = '\t';
	dtTabs[numDT] = '\0';

	//Building the String
	/*int writeLen = strlen(date) + 2 + strlen(title) + strlen(dtTabs) + strlen(time) + 3 + strlen(description) + 1 + 8; //the 8 is to play safety
	char toWrite[writeLen];
	toWrite[0] = '\0';
	strcat(toWrite, date);
	strcat(toWrite, ":\t");
	strcat(toWrite, title);
	strcat(toWrite, dtTabs);
	strcat(toWrite, time);
	strcat(toWrite, "\n\t\t");
	strcat(toWrite, description);
	strcat(toWrite, "\n");

	fputs(toWrite, list);*/

	fputs(date, list);
	fputs(":\t", list);
	fputs(title, list);
	fputs(dtTabs, list);
	fputs(time, list);
	fputs("\n\t\t", list);
	fputs(description, list);
	fputs("\n", list);

	fclose(list);

}


#endif /* LISTHANDLER_H_ */
