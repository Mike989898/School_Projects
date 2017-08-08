/* 
 * bag.c - A Data structure for storing and removing items in a random order
 * 
 * usage: 
 *		To store items of any data type, and to remove these items in a nonspecifc order.
 *		When the extract function is called, a pointer to the inserted object is passed back.
 *
 *
 * Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "bag.h"
/**************** local types ****************/

 typedef struct bag_item{
	void *data;
	struct bag_item *next;
} bag_item_t;

/**************** global types ****************/
typedef struct bag{
	struct bag_item *bag_head;
	void (*itemdelete)(void *data); // deletes data item added to bag by user
} bag_t;

/**************** local function prototypes ****************/
static bag_item_t *new_bag_item(void *data);

/**************** bag_new() ****************/
bag_t *
bag_new(void (*itemdelete)(void *data)){
	bag_t *bag = malloc(sizeof(bag_t));

	if(bag == NULL){
		//not enough space for the bag
		return NULL;
	}
	else{
		//initialize the baghead
		//initialize funtion for deleteing added items
		bag->bag_head = NULL;
		bag->itemdelete = itemdelete;
		return bag;
	}
}

/**************** bag_insert() ****************/
void
bag_insert(bag_t *bag, void *data){
	if(bag == NULL){
		//null bag, can't insert
		return;
	}
	bag_item_t *new_item;
	if ((new_item = new_bag_item(data)) == NULL){
		//not enough space for the new bag item
		return;
	}
	if((bag->bag_head) == NULL){
		bag->bag_head = new_item;
	}
	else{
		//insert item at the front of the bag
		new_item->next = bag->bag_head->next;
		bag->bag_head->next = new_item;
	}

}

/**************** new_bag_item() ****************/
/*helper method for bag_insets
 * creates a new bag_item
 */
static bag_item_t *
new_bag_item(void *data){
	bag_item_t *new_insert = malloc(sizeof(bag_item_t));

	if(new_insert == NULL){
		//not enough room for the new bag item
		return NULL;
	}
	else{
		new_insert->next = NULL;
		new_insert->data = data;
	}
	return new_insert;
}

/**************** bag_extract() ****************/
void *
bag_extract(bag_t *bag){
	void *extract_data; 
	//check for a null bag
	if(bag == NULL){
		return NULL;
	}
	else{
		//check if there's anything in the bag
		if(bag->bag_head == NULL){
			return NULL;
		}
		else if(bag->bag_head->next == NULL){
			//return the data that's in the one item in the bag
			extract_data = bag->bag_head->data;
			//get rid of the bag item in the bag
			free(bag->bag_head);
			bag->bag_head = NULL;
			//user's reponsibility to free the (void *) data
			return extract_data;
		}
		else{
			bag_item_t *remove_item = bag->bag_head->next;
			extract_data = remove_item->data;
			//cut out the item to be removed from the bag
			bag->bag_head->next = remove_item->next;
			free(remove_item);
			//user's reponsibility to free the (void *) data
			return extract_data;
		}
	}
}

/**************** bag_iterate() ****************/
void
bag_iterate(bag_t *bag,
	void (*itemfunc)(void *arg, void *data),
	void *arg)
{
	if (bag == NULL || itemfunc == NULL) {
    return; // bad bag or null function
} 
else {
    // scan the bag
	for (bag_item_t *node = bag->bag_head; node != NULL; node = node->next) {
		(*itemfunc)(arg, node->data); 
	}
}
return;
}

/**************** bag_delete() ****************/
void bag_delete(bag_t *bag){
	if(bag != NULL){
		void *item_to_delete;
		//extract from the bag to get the pointer to the data item
		//then delete the data item
		while((item_to_delete = bag_extract(bag)) != NULL){
			//delete data returned
			(*bag->itemdelete)(item_to_delete);
		}
		//free the bag
		free(bag);
		bag = NULL;
	}
}