/*
 * bag.h - header file for the bag module
 * 
 * John Kotz + Andrew Laub, April 2016
 */

#ifndef __BAG_H
#define __BAG_H

// --------Types--------
typedef struct bag bag_t; // This is the bag type

// ------Functions-------


/**
 * Creates a new bag
 * Will return NULL if something goes wrong!
 */
bag_t *bag_new(void);

/**
 * Inserts a new object into the bag
 */
void bag_insert(bag_t *bag, void *data);

/**
 * Extracts any object from the bag and returns it
 * Returns NULL if there is none
 */
void *bag_extract(bag_t *bag);

/**
 * Frees the bag and all its contents
 */
void bag_delete(bag_t *bag, void (*removeFunc)(void *data));

/**
 * Print the description of a bag
 */
void bag_print_description( bag_t *bag, void (*printfunc)(void *data, FILE* fp), FILE* fp );

/**
 *Iterate over all items in the bag, calling itemfunc(arg, data) for each
 */
void bag_iterate(bag_t *bag, void (*itemfunc)(void* arg, void* data), void* arg);

#endif // __BAG_H
