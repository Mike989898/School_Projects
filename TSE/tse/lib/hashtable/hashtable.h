#ifndef __HASHTABLE_H
#define __HASHTABLE_H

/**************** global types ****************/
typedef struct hashtable hashtable_t;

/****************function prototypes ****************/

/* Create a new (empty) hashtable; 
 * return NULL if error.
 */
hashtable_t *
hashtable_new(const int num_slots, void (*itemdelete)(void *data));

/*looks for a data elements inside the hash with a given key
 *takes a hashtable and a key 
 *returns the data found for the key, and null if the key isn't found
 */
void *hashtable_find(hashtable_t *ht, char *key);

/*inserts an element of data with a given key inside the hash
 *if the element is already inserted or cannot be inserted, returns false
 *takes a hash table, a key, and data to insert
 */
bool hashtable_insert(hashtable_t *ht, char *key, void *data);

/* Iterate over all items in hashtable (in undefined order):
 * call itemfunc for each item, with (arg, key, data).
 */
void hashtable_iterate(hashtable_t *ht, 
		       void (*itemfunc)(void *arg, char *key, void *data),
		       void *arg);

/*Deletes the hashtable
 */
void hashtable_delete(hashtable_t *ht);
#endif // __HASHTABLE_H