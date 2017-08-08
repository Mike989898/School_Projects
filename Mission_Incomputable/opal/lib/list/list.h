/*
 * list.h - header file for a list module
 *
 * John Kotz + Andrew Laub, April 2019
 */

#include <stdbool.h>

#ifndef __LIST_H
#define __LIST_H

// -----Types-----

typedef struct list list_t;

// ------Functions--------

/**
 * Create a new empty list data structure
 */
list_t *list_new(void);

/**
 * Returns data for the given key, or NULL if key is not found.
 */
void *list_find(list_t *list, char *key);

/**
 * Insert key and data into the list
 * Returns false if key already exists, and true if new item was inserted.
 * I return false if the key is NULL
 * If list is NULL, or I encounter other error, return false.
 * Copy the key string, as in our binary-tree example.
 */
bool list_insert(list_t *list, char *key, void *data);

/**
 * Frees the list and everything in it, except for the data
 */
void list_delete(list_t *list);

/**
 *Iterate over all the items in the list, calling itemfunc for each w/ args (arg, key, data)
 */
void list_iterate(list_t* list, void (*itemfunc)(void* arg, char* key, void* data), void* data);

#endif // __LIST_H
