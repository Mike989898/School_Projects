/* 
 * hashtable.c - A Data structure for storing key-value pairs
 * 
 * usage: 
 *		The has table module is used for storing a pieace a data with an associated key.
 *		This particular data structure is optimized for searching, with best case O(1) search time
 *		(provided there are no collisions).
 *
 *
 * Michael Kilgore, April 2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "hashtable.h"

/**************** global types ****************/
typedef struct hashtable{
	int size; //size of hash table
	list_t** data_items; //array of list pointers
} hashtable_t;

/**************** local function prototypes ****************/
static unsigned long JenkinsHash(const char *str, unsigned long mod);

/**************** hashtable_new() ****************/
hashtable_t *
hashtable_new(const int num_slots, void (*itemdelete)(void *data)){
	int i;
	hashtable_t *hashtable = malloc(sizeof(hashtable_t));
	if(hashtable == NULL){
		//not enough space for the hash table
		return NULL;
	}
	else{
		//create table of size specifed by user
		hashtable->size = num_slots;
		//make space for the array of list pointers
		hashtable->data_items = (list_t **) malloc(num_slots*sizeof(list_t *));
		if(hashtable->data_items == NULL){
			//not enough space for data_items pointer
			//free hashtable and return
			free(hashtable);
			return NULL;
		}
		for(i=0; i<num_slots; i++){
			//fill the has with empty lists
			list_t *new_list = list_new(itemdelete);
			hashtable->data_items[i] = new_list;
		}
		return hashtable;
	}
}

/**************** hashtable_find() ****************/
void *
hashtable_find(hashtable_t *ht, char *key){
	if(ht == NULL){
		return NULL;
	}
	unsigned long hash_value = JenkinsHash(key, ht->size);
	//get the list at the location of the hash value
	list_t *check_list = ht->data_items[hash_value];
	void *data;
	//look for the key in the list at the particular hash value
	if((data = list_find(check_list, key)) == NULL){
		//data not found
		return NULL;
	}
	else{
		//found the data in the list, return it
		return data;
	}
}

/**************** hashtable_insert() ****************/
bool 
hashtable_insert(hashtable_t *ht, char *key, void *data){
	//check if the hash table is null
	if(ht == NULL){
		return false;
	}
	unsigned long hash_value = JenkinsHash(key, ht->size);
	//get the list at the location of the hash value
	list_t *insertion_list = ht->data_items[hash_value];
	//note that we don't need to copy the key and 
	//send it to the list_insert function, since the 
	//list_insert function will already copy the key for us
	
	if((list_insert(insertion_list, key, data))){
		//able to insert into the list
		return true;
	}
	else{
		//couldn't insert
		return false;
	}
}

/**************** JenkinsHash() ****************/
/*Hash function provided by professor Kotz
 *note, mod is the size of the hash table
 *(to prevent the hash function from generating values outside the table)
 */
static unsigned long
JenkinsHash(const char *str, unsigned long mod)
{
    if (str == NULL)
      return 0;

    size_t len = strlen(str);
    unsigned long hash, i;

    for (hash = i = 0; i < len; ++i)
    {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % mod;
}

/**************** hashtable_iterate() ****************/
void hashtable_iterate(hashtable_t *ht, 
		       void (*itemfunc)(void *arg, char *key, void *data),
		       void *arg)
{
	if(ht == NULL || itemfunc == NULL){
		return;
	}
	else{
		int i;
		for(i = 0; i < (ht->size); i++){
			//get each of the lists out from the 
			//hash and print them out
			list_iterate(ht->data_items[i], itemfunc, arg);
		}
	}
}

/**************** hashtable_delete() ****************/
void hashtable_delete(hashtable_t *ht){
	if(ht !=NULL){
		//go through the hash table and delete each of the lists
		int i;
		for(i=0; i < (ht->size); i++){
			list_delete(ht->data_items[i]);
			ht->data_items[i] = NULL;
		}
		//free the table of lists
		free(ht->data_items);
		ht->data_items = NULL;
		//free the hashtable object
		free(ht);
		ht = NULL;
	}
}
