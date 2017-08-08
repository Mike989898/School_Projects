#ifndef __BAG_H
#define __BAG_H

/**************** global types ****************/
typedef struct bag bag_t; 

/**************** function prototypes ****************/

/* Create a new (empty) bag; 
 * return NULL if error.
 */
bag_t *bag_new(void (*itemdelete)(void *data));

/* Insert into a bag 
 * provide a a piece of data to be inserted
 */
void bag_insert(bag_t *bag, void *data);

/* Extract from a bag.
 * returns the data extracted.
 */
void *bag_extract(bag_t *bag);

/* Iterate over all items in bag (in undefined order):
 * call itemfunc for each item, passing (arg, data).
 */
void 
bag_iterate(bag_t *bag,
            void (*itemfunc)(void *arg, void *data),
            void *arg);

/*Delete the entire bag
 */
void bag_delete(bag_t *bag);

#endif // __BAG_H