/*
 * hashtable.h - header file for the hashtable module
 * 
 * John Kotz + Andrew Laub, April 2016
 */

#include <stdbool.h>

#ifndef __HASHTABLE_H
#define __HASHTABLE_H

// --------Types--------
typedef struct hashtable hashtable_t; // This is the hasttable type

// ------Functions-------

/**
 * Creates a new empty hash table data structure
 */
hashtable_t *hashtable_new(const int num_slots);

/**
 * Return data for the given key, or NULL if not found
 */
void *hashtable_find(hashtable_t *ht, char *key);

/**
 * Inserts the given item into the hashtable
 * I return false if key already exists, and true if new item was inserted
 * If ht is NULL, or I encounter other error, I return false
 * I also copy the key string
 */
bool hashtable_insert(hashtable_t *ht, char *key, void *data);

/**
 * Freed the hashtable and everything in it, other than the data
 */
void hashtable_delete( hashtable_t *ht, void (*removeFunc)(void *data) );

/**
 * Iterate over all items in hashtable (in undefined order):
 * call itemfunc for each item, with (arg, key, data).
 */
void hashtable_iterate(hashtable_t *ht, 
		       void (*itemfunc)(void *arg, char *key, void *data),
		       void *arg);

/**
 * Merges the second hashtable into the first. 
 * The second is left remains untouched, and all keys are coppied
 * Pass a function in to merge two data
 */
void hashtable_merge(hashtable_t *dest, hashtable_t *source,
		     void (*mergeData)(void **dataDest, void **dataSource));

#endif // __HASHTABLE_H
