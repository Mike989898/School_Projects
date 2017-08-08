/*
 * bag.c - a bag module
 *
 * John Kotz + Andrew Laub, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "../memory/memory.h"
#include "bag.h"

// =======Structs=======
// Local

typedef struct item {
  void *data;
  struct item *next;
} item_t;

// Global

typedef struct bag {
  item_t *first;
} bag_t;

// ======Functions======

bag_t *bag_new(void) {
  bag_t *bag = count_malloc(sizeof(bag_t));

  // If it is not NULL, we should at least initialze it
  if (bag != NULL) {
    bag->first = NULL;
  }

  // If it is NULL, that is exactly what we want to return
  return bag;
}

void bag_insert(bag_t *bag, void *data) {
  if ( bag == NULL ) {
    return;
  }
  // Allocate space for the new thing
  item_t *newItem = count_malloc(sizeof(item_t));
  if ( newItem == NULL ) {
    return;
  }

  newItem->data = data;
  
  // Add to the front of the list
  newItem->next = bag->first;
  bag->first = newItem;
}

void *bag_extract(bag_t *bag) {
  if ( bag == NULL ) {
    return NULL;
  }
  // Save the first item so when I remove it I dont loose it
  item_t *item = bag->first;

  // If the bag is empty
  if (item != NULL) {
    // Now the nextItem could be NULL, but I don't care
    item_t *nextItem = item->next;
    void *data = item->data;
    
    bag->first = nextItem;
    count_free(item);
    return data;
  }
  // Then we return NULL
  return NULL;
}

void bag_delete(bag_t *bag, void (*removeFunc)(void* data)) {
  if (bag == NULL)
    return;

  while (bag->first != NULL) {
    item_t* item = bag->first;
    bag->first = item->next;
    removeFunc(item->data);
    count_free(item);
  }

  count_free(bag);
}

void bag_print_description( bag_t *bag, void (*printfunc)(void *data, FILE* fp), FILE* fp ) {
  if ( bag == NULL ) {
    return;
  }
  fprintf(fp, "{\n");
  item_t* item = bag->first;
  
  while ( item != NULL ) {
    (*printfunc)(item->data, fp);
    item = item->next;
  }

  fprintf(fp, "}\n");
}

void bag_iterate(bag_t* bag, void (*itemfunc)(void* arg, void* data), void* arg) {

  if (bag == NULL)
    return;

  item_t* item = bag->first;

  while( item != NULL ) {
    (*itemfunc)(arg, item->data);
    item = item->next;
  }
}
