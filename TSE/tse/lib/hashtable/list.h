#ifndef __LIST_H
#define __LIST_H

/**************** global types ****************/
typedef struct list list_t;

/**************** functions ****************/

/* Create a new (empty) list; 
 * return NULL if error.
 */
list_t *list_new(void (*itemdelete)(void *data));

/*Finds an node in the list containing a particular key
 *Returns null if not found or the list is empty
 *takes a list and a key
 */
void *list_find(list_t *list, char *key);

/*Inserts into the list
 *returns false if the list is NULL or
 *the key is already in the list.
 *takes a list, a key, and data to insert
 */
bool list_insert(list_t *list, char *key, void *data);

/* Iterate over all items in list (in undefined order):
 * call itemfunc for each item, passing (arg, key, data).
 */
void list_iterate(list_t *list,
		  void (*itemfunc)(void *arg, char *key, void *data),
		  void *arg);

/*Deletes the passed list
 */
void list_delete(list_t *list);

#endif // __LIST_H