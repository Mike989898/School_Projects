/*
 *counterstest.c- A Program for testing the counters data structure
 *usage: 
 *		Testing all the opperations of the counters data structure to see that 
 *		they all function properly.
 *
 *Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"

/*Prototypes*/
void printCounters(void *arg, int key, int count);

int main(){
	counters_t *myCounter = counters_new();

	//add a few counters
	counters_add(myCounter, 1);
	counters_add(myCounter, 1);
	counters_add(myCounter, 2);
	counters_add(myCounter, 2);
	counters_add(myCounter, 2);
	counters_add(myCounter, 3);
	counters_add(myCounter, 3);
	counters_add(myCounter, 4);
	counters_add(myCounter, 1);
	counters_add(myCounter, 1);
	counters_add(myCounter, 1);
	counters_add(myCounter, 1);

	//print out the counter values
	printf("The count for Key %d is %d\n", 1, counters_get(myCounter, 1));
	printf("The count for Key %d is %d\n", 2, counters_get(myCounter, 2));
	printf("The count for Key %d is %d\n", 3, counters_get(myCounter, 3));
	printf("The count for Key %d is %d\n", 4, counters_get(myCounter, 4));
	printf("The count for Key %d is %d\n", 5, counters_get(myCounter, 5));

	//test counters_get on null counter(shouldn't segfault)
	counters_get(NULL, 0);

	//create a null counters object and make sure that delete and insert
	//don't segfault
	counters_t *null_counters = NULL;
	counters_add(null_counters, 1);
	counters_delete(null_counters);

	counters_iterate(myCounter, printCounters, NULL);
	printf("\n");

	//check that we are able to set a couple of the counters
	counters_set(myCounter, 1, 2);
	counters_set(myCounter, 2, 10);
	counters_set(myCounter, 5, 5);

	//iterate through the counters obj and print out key-value pairs
	counters_iterate(myCounter, printCounters, NULL);	

	//check that the delete opperation works properly
	counters_delete(myCounter);

	printf("\nCounters2: \n");
	counters_t *counter2 = counters_new();
	counters_set(counter2, 6, 6);
	counters_iterate(counter2, printCounters, NULL);	
	counters_delete(counter2);
}

//function for printing out key-value pairs of the counter
void printCounters(void *arg, int key, int count){
	printf("The key is: %d and the count is: %d\n", key, count);
}