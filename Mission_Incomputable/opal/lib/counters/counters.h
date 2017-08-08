/* counters.h - header file for the counters module
 * 
 * John Kotz + Andrew Laub, April 2016
 */

#ifndef __COUNTERS_H
#define __COUNTERS_H

// --------Types--------
typedef struct counters counters_t; // This is the bag type

// ------Functions-------

/**
 * Create a new empty counters object
 */
counters_t *counters_new(void);

/**
 * Add a key to the counters
 * If a key exists, increment its counter; otherwise add key and set its counter to 1
 */
void counters_add(counters_t *ctrs, int key);

/**
 * Returns the current value of the counter for key, or 0 if key is not found.
 */
int counters_get(counters_t *ctrs, int key);

/**
 * Delete the whole data structure, and its contents
 */
void counters_delete(counters_t *ctrs);

/**
 *Set value of the counter w/ key key to value "count"
 */
void counters_set(counters_t* ctrs, int key, int count);

/**
 *Iterate over all counters, calling itemfunc on each item, w/ args arg, key, and count
 */
void counters_iterate(counters_t *ctrs, void (*itemfunc)(void*arg, int key, int count), void* arg);



#endif
