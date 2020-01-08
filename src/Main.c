/*
 * Main.c
 *
 *  Created on: Dec 24, 2019
 *      Author: Krishna Prajapati
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "LinkedList.h"
#include "ListHandler.h"


int main(void){
	setbuf(stdout, NULL);

    //Get a valid List file
    char* filename = (char *) malloc(MAX_LINE_LENGTH*sizeof(char));
    char* input = (char *) malloc(MAX_LINE_LENGTH*sizeof(char));
    FILE* list;

    do{
        getInp(filename, "Enter the location of the To-Do list file (enter d for default): ");
        if(filename == NULL || strcmp(filename, "d")== 0|| strcmp(filename, "D")==0){
            filename = "list.txt";
        }
        list = fopen(filename, "r");

        if(list == NULL) {
            printf("List was invalid\n");
            fclose(list);
        }
    } while (list == NULL);
    struct DayNode* listRoot = buildList(list);
    fclose(list);

    printList(&listRoot);

    //printList(filename);

    //Main loop where user enters actions to perform
    int loop = 1;
    while(loop == 1){
        getInp(input, "\nWould you like to ADD an entry (a), CHECK off/CHANGE an entry (c), SEE the list (s), or QUIT (q)? ");
        if(input == NULL){
            printf("Null entry \n");
            continue;
        }
        switch(input[0]){
        case 'a':
        case 'A':
            //Add an entry
            addEntry(filename);
            break;
        case 'c':
        case 'C':
            //Check/Complete an existing item in list
            break;
        case 's':
        case 'S':
            printList(&listRoot);
            break;
        case 'q':
        case 'Q':
            printf("Saving and exiting...");
            loop--;
            break;
        default:
            printf("That is an invalid option\n");
            break;
        }
        input[0] = '\0';
    }

    free(input);
    free(filename);
    printf(" Done!\n Press enter to close...");
    getchar();
    return 0;
}

