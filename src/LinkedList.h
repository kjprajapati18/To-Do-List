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

void fixPriority(struct DayNode* day);
void changeTaskPriority(struct DayNode* day, struct TaskNode* task, int priority);
void addTask(struct DayNode **target, int priority, char* title, char* timeDue, char* description);

void userAddEntry(struct DayNode** root);
void userCheckEntry(struct DayNode** root);
void userEditEntry(struct DayNode **root);

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

		if(readLine[0] == 'U' && readLine[1] == ':'){ //Set day ptr to the root(which is the unordered list)
			ptr = root;
			continue;

		} else if (isdigit(readLine[0])){ //If the line starts with a number, then it must be a date
			free(date);
			date = findDate(readLine);
			ptr = addDay(&root, date[0], date[1], date[2]);
			continue;

		} else {
			strcpy(title, readLine); //else, there is no date change, just add to the same date as previous iteration
		}

		if(fgets(time, MAX_LINE_LENGTH, file) == NULL) break; //break on any errors
		if(fgets(description, MAX_LINE_LENGTH, file) == NULL) break;
		priority = ptr->size + 1; //Priority is set in ascending order (smallest number = first in list)
		addTask(&ptr, priority, title, time, description);
	}

	free(date);
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
		if(priority == newTask->next->priority) fixPriority(dPtr);
		return;
	}

	//We need to start checking the next few priorities. Note that if length equals 1, then it will not go to loop
	struct TaskNode *tPtr = dPtr->first;
	while(tPtr->next != NULL){
		if(tPtr->next->priority >= priority){
			newTask->next = tPtr->next;
			tPtr->next = newTask;
			if(priority == newTask->next->priority) fixPriority(dPtr);
			return;
		}
		tPtr = tPtr->next;
	}

	//Task is the lowest priority in the list
	tPtr->next = newTask;
}

void changeTaskPriority(struct DayNode* day, struct TaskNode* task, int priority){
	if(day->first == task){
		day->first = task->next;
		day->size--;
		addTask(&day, priority, task->title, task->timeDue, task->description);
		free(task);
		return;
	}

	struct TaskNode* tPtr = day->first;
	while(tPtr->next != task && tPtr != NULL){
		tPtr = tPtr->next;
	}

	if(tPtr == NULL){
		printf("There was an error.");
		return;
	}

	tPtr->next = task->next;
	addTask(&day, priority, task->title, task->timeDue, task->description);
	free(task);
	fixPriority(day);

}

//Fixes the priorities of each item afterwards to maintain hierarchy
void fixPriority(struct DayNode* day){

	struct TaskNode* ptr = day->first;
	int count = 1;
	while(ptr != NULL){
		ptr->priority = count;
		count++;
		ptr = ptr->next;
	}
}


//Gets User input and prepares for task to be added
void userAddEntry(struct DayNode** root){
	//Initialize variables needed
	struct DayNode* dPtr;
    char date[MAX_LINE_LENGTH];
    char title[MAX_LINE_LENGTH];
    char timeDue[20];
    char description[MAX_LINE_LENGTH];
    char priorityStr[10];
    char* ePtr;
    int priority;
    int* day = NULL;
    date[0] = '\0';
    title[0] = '\0';
    timeDue[0] = '\0';
    description[0] = '\0';

    //Request inputs
    do{
    	getInp(date, "What date would you like to add an entry for? (enter U for unordered list, mm/dd/yy for that date, or B to back): ");
    	if(date[0] == 'b' || date[0] == 'B') return;
    	day = findDate(date);
    	if(day==NULL) printf("That is an invalid date...\n\n");
    }while(day == NULL);

    getInp(title, "Please enter a title: ");
    getInp(timeDue, "Enter a time due: ");
    getInp(description, "Enter a description: ");

    do{
    	getInp(priorityStr, "What number entry should this be for that day (1= first entry, 2= second, etc.): ");
    	priority = strtol(priorityStr, &ePtr, 10);
    }while(priority < 1 || strcmp(priorityStr, ePtr) == 0);

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


    //0 means that its a part of the unordered list*
    if(day[0] == 0){
        addTask(root, priority, title, timeDue, description);
    } else {
    	dPtr = addDay(root, day[0], day[1], day[2]);
    	addTask(&dPtr, priority, title, timeDue, description);
    }
    free(day);
}

void userCheckEntry(struct DayNode** root){

	char input[MAX_LINE_LENGTH];
	char* ePtr;
	struct DayNode* dPtr = *root;
	struct TaskNode* tPtr;
	int* date = NULL;
	int taskNumber = 0;

	//Get the day. Give the option to back out
	do{
		getInp(input, "What day would you like to check off an entry for (U for unordered, B to back): ");
		if(input[0] == 'b' || input[0] == 'B') return;
		date = findDate(input);
		if(date == NULL) printf("That is an invalid date...\n\n");
	} while(date == NULL);

	while(dPtr != NULL){
		if(dPtr->day == date[0] && dPtr->month == date[1] && dPtr->year == date[2]) break;
		dPtr = dPtr->next;
	}
	free(date);

	if(dPtr == NULL || dPtr->size == 0){
		printf("\nThere are no tasks in this day to complete!\n");
		return;
	}

	//Now that we have the day, find the task
	if(dPtr->day == 0) printf("You have selected the unordered list and there are %d entries.\n", dPtr->size);
	else printf("\nThe date you have selected is %d/%d/%d and there are %d entries.\n", dPtr->month, dPtr->day, dPtr->year, dPtr->size);
	do{
		getInp(input, "What number task would you like to check off (1=first, 2=second, etc., B to back): ");
		if(input[0] == 'b' || input[0] == 'B') return;
		taskNumber = (int) strtol(input, &ePtr, 10);

		if(taskNumber < 1 || taskNumber > dPtr->size) printf("Invalid number. Please try again or enter B to back...\n\n");
	}while(taskNumber < 1 || taskNumber > dPtr->size);

	taskNumber--;
	tPtr = dPtr->first;
	if(taskNumber == 0){
		dPtr->first = tPtr->next;
		free(tPtr);
		return;
	}

	while(taskNumber>1){
		tPtr = tPtr->next;
		taskNumber--;
	}

	struct TaskNode* temp = tPtr->next;
	tPtr->next = temp->next;
	free(temp);
	dPtr->size--;
	fixPriority(dPtr);
}

//Allows user to edit task entries
void userEditEntry(struct DayNode **root){
	char input[MAX_LINE_LENGTH];
	char* ePtr;
	struct DayNode* dPtr = *root;
	struct TaskNode* tPtr;
	int* date;
	int taskNumber = 0;

	//Get the day. Give the option to back out
	do{
		getInp(input, "What day would you like to edit an entry for (U for unordered, B to back): ");
		if(input[0] == 'b' || input[0] == 'B') return;
		date = findDate(input);
		if(date == NULL) printf("That is an invalid date...\n\n");
	} while(date == NULL);

	while(dPtr != NULL){
		if(dPtr->day == date[0] && dPtr->month == date[1] && dPtr->year == date[2]) break;
		dPtr = dPtr->next;
	}
	free(date);
	if(dPtr == NULL || dPtr->size == 0){
		printf("\nThere are no tasks in this day to edit!\n");
		return;
	}


	//Now that we have the day, find the task
	do{
		if(dPtr->day == 0) printf("You have selected the unordered list and there are %d entries.\n", dPtr->size);
		else printf("The date you have selected is %d/%d/%d and there are %d entries.\n", dPtr->month, dPtr->day, dPtr->year, dPtr->size);
		getInp(input, "What number task would you like to edit (1=first, 2=second, etc., B to back): ");

		if(input[0] == 'b' || input[0] == 'B') return;

		taskNumber = (int) strtol(input, &ePtr, 10);
		if(taskNumber < 1 || taskNumber > dPtr->size) printf("Invalid number. Please try again or enter B to back...\n");

	}while(taskNumber < 1 || taskNumber > dPtr->size);

	taskNumber--;
	tPtr = dPtr->first;

	while(taskNumber>0){
		tPtr = tPtr->next;
		taskNumber--;
	}

	int loop = 1, prior = 0;
	char temp[MAX_LINE_LENGTH];

	while(loop !=0){
		temp[0] = '\t';
		temp[1] = '\0';
		getInp(input, "Would you like to edit the Title (T), the time due (M), the description (D), or the priority (P)? Enter B to go back: ");

		switch(input[0]){
		case 't':
		case 'T':
			printf("The current title is as follows: \n\t%s\n\n", tPtr->title);
			getInp(input, "Enter a new title or B to go back: ");
			if(input[0] == 'b' || input[0] == 'B') break;
			strcpy(tPtr->title, strcat(input, "\n"));
			break;

		case 'm':
		case 'M':
			printf("The current time due is as follows: \n%s\n\n", tPtr->timeDue);
			getInp(input, "Enter a new time due, or B to go back: ");
			if(input[0] == 'b' || input[0] == 'B') break;

			strcat(temp, input);
			strcat(temp, "\n");
			strcpy(tPtr->timeDue, temp);
			break;

		case 'd':
		case 'D':
			printf("The current description is as follows: \n%s\n\n", tPtr->description);
			getInp(input, "Enter a new description or B to go back: ");
			if(input[0] == 'b' || input[0] == 'B') break;

			strcat(temp, input);
			strcat(temp, "\n");
			strcpy(tPtr->description, temp);
			break;

		case 'p':
		case 'P':
			getInp(input, "Enter a new priority or B to go back: ");
			if(input[0] == 'b' || input[0] == 'B') break;
			prior = (int) strtol(input, &ePtr, 10);
			if(prior == 0 && strcmp(ePtr, input) == 0){
				printf("That is not a valid input");
				break;
			}
			changeTaskPriority(dPtr, tPtr, prior);
			break;

		case 'b':
		case 'B':
			loop--;
			break;
		default:
			printf("That is an invalid response... \n");

		}
	}

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
			fputs("U:\n", write);
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
