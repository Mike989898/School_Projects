/* 
 * counters.c - A Data structure for storing keys and the amount of times they've been added
 * 
 * usage: 
 *		Adding a key to the counters structure causes it to make space for the new
 *		key if it hasn't been added yet, and increment the counter assoicated with the key
 *		if it has already been added.
 *
 *
 * Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"

/**************** local types ****************/
 typedef struct counters_node{
 	int key;
	int counter;
	struct counters_node *next;
} counters_node_t;

/**************** global types ****************/
typedef struct counters{
	struct counters_node *counters_head;
} counters_t;

/**************** local function prototypes ****************/
static counters_node_t *new_counter_node(int key);

/**************** counters_new() ****************/
counters_t *
counters_new(void){
	counters_t *counters = malloc(sizeof(counters_t));
	if(counters == NULL){
		//not enough space for the new counters object
		return NULL;
	}
	else{
		counters->counters_head = NULL;
		return counters;
	}
}

/**************** counters_add() ****************/
void 
counters_add(counters_t *ctrs, int key){
	if(ctrs == NULL){
		//null counters object, return
		return;
	}

	counters_node_t *search_node;
	if((search_node = ctrs->counters_head)== NULL){
		//no counters currently in the counters obj, add a new one
		counters_node_t *new_counter;
		if((new_counter = new_counter_node(key)) == NULL){
		 	//not enough space for the new counter node, return
		 	return;
		}
		else{
			//set the head
			ctrs->counters_head = new_counter;
			return;
		}
	}
	while(search_node != NULL){
		if(search_node->key == key){
			//found the key
			//increment the counter and break out of the loop
			++(search_node->counter);
			return;
		}
		search_node = search_node->next;
	}
	//key wasn't there, add key and counter
	counters_node_t *new_counter2;
	if((new_counter2 = new_counter_node(key)) == NULL){
		 //not enough space for the new counter node, return
		 return;
	}
	else{
		//insert new counter
		new_counter2->next = ctrs->counters_head->next;
		ctrs->counters_head->next = new_counter2;
	}

}

/**************** new_counter_node() ****************/
/*A helper method for creating
 *a new counter node.
 *returns a counter_node_t
 */
static counters_node_t *
new_counter_node(int key){
	counters_node_t *new_insert = malloc(sizeof(counters_node_t));
	if(new_insert == NULL){
		//not enough room for the new node
		return NULL;
	}
	else{
		new_insert->key = key;
		new_insert->counter = 1;//increment count after adding new element
		new_insert->next = NULL;
	}
	return new_insert;
}

void counters_delete(counters_t *ctrs){
	if(ctrs == NULL){
		//Null object, nothing to delete
		return;
	}
	counters_node_t *current = ctrs->counters_head;
	counters_node_t *next;
	while(current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	ctrs->counters_head = NULL;
	free(ctrs);
	ctrs = NULL;
}

/**************** counters_get() ****************/
/*A method for getting the count
 *of a perticular key. 
 *takes: a counters_t object
 */
int counters_get(counters_t *ctrs, int key){
	if(ctrs == NULL){
		//null counters obj, so the counter can't be found
		return 0;
	}

	counters_node_t *curr = ctrs->counters_head;
	while(curr != NULL){
		if(curr->key == key){
			//if we found the counter, return the count
			return (curr->counter);
		}
		curr = curr->next;
	}
	//counter not found
	return 0;
}

/**************** counters_set() ****************/
void counters_set(counters_t *ctrs, int key, int count){
	if(ctrs == NULL){
		return;
	}
	// if the count isn't there add it. 
	// We still have to reset the count.
	if((counters_get(ctrs, key)) == 0){
		counters_add(ctrs, key);
	}

	//the counter was found
	//find the appropriate node
	//and update its count
	counters_node_t *curr = ctrs->counters_head;
	while(curr != NULL){
		if(curr->key == key){
			curr->counter = count;
			break;
		}
		curr = curr->next;
	}
}

/**************** counters_iterate() ****************/
void counters_iterate(counters_t *ctrs,
	void (*itemfunc)(void *arg, int key, int count),
	void *arg)
{
	if (ctrs == NULL || itemfunc == NULL) {
    return; // null counters or item function
} 
else {
    // scan the counters obj
	for (counters_node_t *node = ctrs->counters_head; node != NULL; node = node->next) {
		(*itemfunc)(arg, node->key, node->counter); 
	}
}
return;
}

/**************** print_counters() ****************/
/*A method for printing
 *out the key-value pair structure of 
 *the counter's object
 *for testing and debugging purposes
 */
void print_counters(counters_t *ctrs){
	if(ctrs == NULL){
		return;
	}
	if(ctrs->counters_head == NULL){
		return;
	}
	counters_node_t *current = ctrs->counters_head;
	printf("Key   Count\n");
	while(current != NULL){
		printf("%d\t%d\n", current->key, current->counter);
		current = current->next;
	}
}

