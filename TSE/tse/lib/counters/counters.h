#ifndef __COUNTERS_H
#define __COUNTERS_H

/**************** global types ****************/
typedef struct counters counters_t;

/****************function prototypes ****************/

/* Create a new (empty) counters object; 
 * return NULL if error.
 */
counters_t *counters_new(void);

/*Searchers for a counter with a particular key
 *if the key exists, increment it, if not, add a new key 
 *and corresponding counter
 *take a counters object and a search key
 */
void counters_add(counters_t *ctrs, int key);

/*Delete counters
 *take a counters object
 */
void counters_delete(counters_t *ctrs);

/* Get the value of a counter for a 
 * given key
 */
int counters_get(counters_t *ctrs, int key);

/* Set value of the counter indicated by key.
 * If the key does not yet exist, create a counter for it.
 * NULL counters is ignored.
 */
void counters_set(counters_t *ctrs, int key, int count);

/* Iterate over all counters in the set (in undefined order):
 * call itemfunc for each item, with (arg, key, count).
 */
void counters_iterate(counters_t *ctrs,
		      void (*itemfunc)(void *arg, int key, int count),
		      void *arg);

/*prints the key-value paired list of counters(for debugging and testing)
 *takes a counters object
 */
void print_counters(counters_t *ctrs);

#endif // __COUNTERS_H