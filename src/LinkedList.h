/*
 * LinkedList.h
 *
 *  Created on: Dec 29, 2019
 *      Author: kjpra
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "ListHandler.h"

struct DayNode{

	int day;
	int month;
	int year;

	int size;

	struct DayNode *next;
	struct TaskNode *first;

};

struct TaskNode{

	int priority;
	char title[MAX_LINE_LENGTH];
	char timeDue[MAX_LINE_LENGTH];
	char description[MAX_LINE_LENGTH];

	struct TaskNode *next;

};


struct DayNode* init();
struct DayNode* addDay(struct DayNode **root, int day, int month, int year);
struct DayNode* buildList(FILE* file);

void incPriorities(struct TaskNode* start);
void addTask(struct DayNode **target, int priority, char* title, char* timeDue, char* description);
void userAddEntry(struct DayNode* root);

void freeList(struct DayNode** root);
void printList(struct DayNode** root);
void saveList(struct DayNode** root, char* filename);



struct DayNode* init(){
	struct DayNode *newDay = (struct DayNode*) malloc(sizeof(struct DayNode));
	newDay->day = 0;
	newDay->month = 0;
	newDay->year = 0;
	newDay->size = 0;
	newDay->first = NULL;
	newDay->next = NULL;
	return newDay;

}

//Does not allow duplicates, returns a DayNode so that we can either use the new DayNode an existing one, meaning it double functions as a search
struct DayNode* addDay(struct DayNode **root, int day, int month, int year){

	if(day == 0) return *root;

	//Initialize the new Node
	struct DayNode *ptr = *root;
	struct DayNode *newDay = (struct DayNode*) malloc(sizeof(struct DayNode));
	struct DayNode *next = ptr->next;
	newDay->day = day;
	newDay->month = month;
	newDay->year = year;
	newDay->size = 0;
	newDay->first = NULL;
	newDay->next = NULL;

	//Note that the root never needs to be changed since it is initialized to "U" (unordered list first)
	//Technically this means we can use *root instead of **root
	while(next != NULL){
		//If the new date precedes the next LL item, then we're going to insert at this spot

		if(next->year > year || (next->month > month && next->year == year) || (next->day >= day && next->month == month && next->year == year)){

			//Check if the day is already there;
			if(next->day == day && next->month == month && next->year == year){
				free(newDay);
				return next;
			}

			//Add if its not already there
			newDay->next = next;
			ptr->next = newDay;
			return newDay;
		}

		ptr = next;
		next = next->next;
	}

	ptr->next = newDay;
	return newDay;
}


struct DayNode* buildList(FILE* file){

	char readLine[MAX_LINE_LENGTH]; //Used to check if tasks need to be added at a differnet part of LL
	char title[MAX_LINE_LENGTH];
	char time[MAX_LINE_LENGTH];
	char description[MAX_LINE_LENGTH];
	readLine[0] = '\0';
	title[0] = '\0';
	time[0] = '\0';
	description[0] = '\0';

	int* date = NULL; //day, month, year
	int priority;

	struct DayNode* root = init();
	struct DayNode* ptr = root;

	while(fgets(readLine, MAX_LINE_LENGTH, file) != NULL){

		if(readLine[0] == '_' || readLine[0] == '\n' || (readLine[0] == '\t' && (readLine[1] == '\n' || readLine[1] == '\t')) ){
			//Underscores and blank spaces after dates/tasks are used for visual purposes, skip over these
			continue;
		}

		if(strcmp(readLine, "U:\n") == 0){ //Set day ptr to the root(which is the unordered list)
			ptr = root;
			fgets(title, MAX_LINE_LENGTH, file); //The next line after a date will always be a ________
			if(fgets(title, MAX_LINE_LENGTH, file) == NULL) break; //Get title

		} else if (isdigit(readLine[0])){ //If the line starts with a number, then it must be a date
			free(date);
			date = findDate(readLine);
			ptr = addDay(&root, date[0], date[1], date[2]);
			fgets(title, MAX_LINE_LENGTH, file); //The next line after a date will always be a ________
			if(fgets(title, MAX_LINE_LENGTH, file) == NULL) break;

		} else {
			strcpy(title, readLine); //else, there is no date change, just add to the same date as previous iteration
		}

		if(fgets(time, MAX_LINE_LENGTH, file) == NULL) break; //break on any errors
		if(fgets(description, MAX_LINE_LENGTH, file) == NULL) break;
		priority = ptr->size + 1; //Priority is set in ascending order (smallest number = first in list)
		addTask(&ptr, priority, title, time, description);
	}

	if(date != NULL) free(date);
	return root; //Return the root, which contains the whole list in LL format
}

//Adds Task to LinkedList structure
void addTask(struct DayNode **target, int priority, char* title, char* timeDue, char* description){

	struct TaskNode *newTask = (struct TaskNode*) malloc(sizeof(struct TaskNode));
	strcpy(newTask->title,title);
	strcpy(newTask->timeDue, timeDue);
	strcpy(newTask->description, description);
	newTask->priority = priority;
	newTask->next = NULL;

	struct DayNode *dPtr = *target;
	dPtr->size++;

	//If there are no tasks in this day, then no need to check priorities
	if(dPtr->size == 1){
		dPtr->first = newTask;
		return;
	}
	//For lengths != 0, we can always check the first element and add front if its lower priority than the new task
	if(dPtr->first->priority >= priority){
		newTask->next = dPtr->first;
		dPtr->first = newTask;
		if(priority == newTask->next->priority) incPriorities(newTask->next);
		return;
	}

	//We need to start checking the next few priorities. Note that if length equals 1, then it will not go to loop
	struct TaskNode *tPtr = dPtr->first;
	while(tPtr->next != NULL){
		if(tPtr->next->priority >= priority){
			newTask->next = tPtr->next;
			tPtr->next = newTask;
			if(priority == newTask->next->priority) incPriorities(newTask->next);
			return;
		}
		tPtr = tPtr->next;
	}

	//Task is the lowest priority in the list
	tPtr->next = newTask;
}

//Increments the priorities of each item afterwards to maintain hierarchy
void incPriorities(struct TaskNode* start){
	struct TaskNode* ptr = start;
	while(ptr != NULL){
		ptr->priority++;
		ptr = ptr->next;
	}
}

//Gets User input and prepares for task to be added
void userAddEntry(struct DayNode* root){
	//Initialize variables needed
	struct DayNode* dPtr;
    char date[MAX_LINE_LENGTH];
    char title[MAX_LINE_LENGTH];
    char timeDue[20];
    char description[MAX_LINE_LENGTH];
    char priorityStr[10];
    int priority;
    date[0] = '\0';
    title[0] = '\0';
    timeDue[0] = '\0';
    description[0] = '\0';

    //Request inputs
    getInp(date, "What date would you like to add an entry for (enter U for unordered list, else mm/dd/yy)? ");
    getInp(title, "Please enter a title: ");
    getInp(timeDue, "Enter a time due: ");
    getInp(description, "Enter a description: ");
    getInp(priorityStr, "What number entry should this be for that day (1= first entry, 2= second, etc.): ");
    priority = atoi(priorityStr);

    //Find which day to put this entry
    int* day = findDate(date);

    //Add spaces to match formatting
    int titleLen = strlen(title);
    int timeLen = strlen(timeDue);
    title[titleLen] = '\n';
    title[titleLen+1] = '\t';
    title[titleLen+2] = '\0';
    timeDue[timeLen] = '\n';
    timeDue[timeLen+1] = '\t';
    timeDue[timeLen+2] = '\0';
    description[strlen(description)+1] = '\0';
    description[strlen(description)] = '\n';


    //-1 means that its a part of the unordered list*
    if((*day) == -1){
        printf("good");
    	//writeEntry("U", title, timeDue, description, filename);
    } else {
    	dPtr = addDay(&root, day[0], day[1], day[2]);
    	addTask(&dPtr, priority, title, timeDue, description);
    }
    free(day);
}

//Print current LinkedList
void printList(struct DayNode **root){

	struct DayNode* dPtr = *root;
	struct TaskNode* tPtr;

	while(dPtr != NULL){
		if(dPtr->day == 0){
			printf("U: \n");
		} else {
			printf("%d/%d/%d\n", dPtr->month, dPtr->day, dPtr->year);
		}
		printLine();

		tPtr = dPtr->first;
		while(tPtr != NULL){
			printf(tPtr->title);
			printf(tPtr->timeDue);
			printf(tPtr->description);
			tPtr = tPtr->next;
		}

		printf("\n");
		printLine();
		dPtr = dPtr->next;
	}

	printf("END\n");
}

//Saves the list to the text file it is used to read the list
void saveList(struct DayNode** root, char* filename){
	struct DayNode* dPtr = *root;
	struct TaskNode* tPtr;

	FILE* write = fopen(filename, "w");

	while(dPtr != NULL){
		if(dPtr->day == 0){
			fputs("U: \n", write);
		} else {
			char date[10];
			sprintf(date, "%d/%d/%d\n", dPtr->month, dPtr->day, dPtr->year);
			fputs(date, write);
		}
		fputs("__________________________________________________________________________________________________\n", write);

		tPtr = dPtr->first;
		while(tPtr != NULL){
			fputs(tPtr->title, write);
			fputs(tPtr->timeDue, write);
			fputs(tPtr->description, write);
			tPtr = tPtr->next;
		}

		fputs("\n__________________________________________________________________________________________________\n", write);
		dPtr = dPtr->next;
	}

	fclose(write);
}


/* This will free the LinkedLists. It starts with the first day, which is the given argument.
 * On the given day, it will first remove tasks 2 to the end if they exist. It will then remove the first task if it exists.
 * After removing all the tasks on a given day, it will start removing tasks on the next day in the same order described.
 * The days are removed recursively. The last day is freed first, and the first day is freed last.
 */
void freeList(struct DayNode** root){
	if((*root) == NULL){ //Base case since we will use recursion
		return;
	}

	struct TaskNode* ptr = (*root)->first;
	struct TaskNode* next;

	if(ptr == NULL){
		//If there is nothing in the list, then we just need to free the Day
		freeList(&((*root)->next)); //Recurse on the next day
		free((*root)); //Free this day
		return;

	} else{
		next = ptr->next;
	}

	while(next != NULL){ //Free tasks 2 through the end first
		ptr->next = next->next;
		free(next);
		next = ptr->next;
	}
	free(ptr); //Free the first task (since we know at this point that there is only 1 element)
	freeList(&((*root)->next)); //Recurse on next day
	free((*root)); //Free the current day
}

#endif /* LINKEDLIST_H_ */
