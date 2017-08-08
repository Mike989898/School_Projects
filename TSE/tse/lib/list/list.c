/* 
 * list.c - A Data structure for storing items and then searching for them
 * 
 * usage: 
 *		To store items of any data type, and to find these items(at the user's request) 
 *      by a given key.	
 *
 *
 * Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"

/**************** local types ****************/
 typedef struct list_node{
 	char *key;
	void *data;
	struct list_node *next;
} list_node_t;

/**************** global types ****************/
typedef struct list{
	struct list_node *list_head;
	void (*itemdelete)(void *data); // deletes data item added to bag by user
} list_t;

/**************** local function prototypes ****************/
static list_node_t *new_node_create(char *key, void *data);

/**************** list_new() ****************/
list_t *
list_new(void (*itemdelete)(void *data)){
	list_t *list = malloc(sizeof(list_t));
	if(list == NULL){
		//not enough space for the new list
		return NULL;
	}
	else{
		list->list_head = NULL;
		list->itemdelete = itemdelete;
		return list;
	}
}

/**************** list_find() ****************/
void *
list_find(list_t *list, char *key){
	if(list == NULL){
		//empty list
		return NULL;
	}
	list_node_t *search_node = list->list_head;
	while(search_node !=NULL){
		if(strcmp(key, search_node->key) == 0){
			//key was found, return the data
			return search_node->data;
		}
		//keep iterating through the list if the key isn't found
		search_node = search_node->next;
	}
	//no data for the given key was found
	return NULL;
}

//helper method that creates a new list node
static list_node_t *
new_node_create(char *key, void *data){
	list_node_t *new_insert = malloc(sizeof(list_node_t));
	if(new_insert == NULL){
		//not enough room for the new list item
		return NULL;
	}
	else{
		new_insert->key = malloc(strlen(key)+1);
		if(new_insert->key == NULL){
			//not enough space for key, do some cleanup
			free(new_insert);
			return NULL;
		}
		else{
			strcpy(new_insert->key, key);
			new_insert->next = NULL;
			new_insert->data = data;
			return new_insert;
		}
	}
}

/**************** list_insert() ****************/
bool 
list_insert(list_t *list, char *key, void *data){
	//check if the list is null
	if(list == NULL){
		return false;
	}

	if((list_find(list, key)) == NULL){
		list_node_t *new_node;
		if ((new_node = new_node_create(key, data)) == NULL){
			//not enough space for the new node
			return false;
		}
		else{
			if((list->list_head) == NULL){
				//new node is now the head
				list->list_head = new_node;
			}
			else{
				//insert item at the front of the list
				new_node->next = list->list_head->next;
				list->list_head->next = new_node;
			}
			return true;
		}
	}
	else{
		//already a node in the list with the current key
		//don't add duplicates
		return false;
	}
}

/**************** list_iterate() ****************/
void list_iterate(list_t *list,
	void (*itemfunc)(void *arg, char *key, void *data),
	void *arg)
{
	if (list == NULL || itemfunc == NULL) {
		return; // null list or item function
	} 
	else {
    	// scan the list 
		for (list_node_t *node = list->list_head; node != NULL; node = node->next) {
			(*itemfunc)(arg, node->key, node->data); 
		}
	}
	return;
}

/**************** list_delete() ****************/
void list_delete(list_t *list){
	if(list != NULL){
		//iterate through the list and free the
		//data the node contains
	list_node_t *current = list->list_head;
	list_node_t *next;
	while(current != NULL){
		next = current->next;
		(*list->itemdelete)(current->data);
		free(current->key);
		free(current);
		current = next;
	}
	list->list_head = NULL;
	free(list);
	list = NULL;
	}
}



