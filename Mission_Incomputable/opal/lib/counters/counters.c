/*
 * counters.c - a counters module
 *
 * John Kotz + Andrew Laub, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "../memory/memory.h"
#include "counters.h"

// =======Structs=======
// Local

typedef struct item {
  struct item *next;
  struct item *prev;
  int key;
  int count;
} item_t;

// Global

typedef struct counters {
  item_t *first;
} counters_t;

// ------Functions-------

counters_t *counters_new(void) {
  counters_t *counters = count_malloc(sizeof(counters_t));

  if ( counters != NULL )
    counters->first = NULL;
  
  return counters;
}

void counters_add(counters_t *ctrs, int key) {
  if ( ctrs == NULL ) {
    return;
  }

  item_t *prvNode = NULL;
  item_t *node = ctrs->first;

  if ( ctrs->first == NULL ) {
    // Its empty, so of course the key is not here
    item_t *newNode = count_malloc(sizeof(item_t));
    newNode->count = 1;
    newNode->key = key;
    newNode->next = NULL;

    // There is no previous
    newNode->prev = NULL;
    ctrs->first = newNode;
    return;
  }

  while ( node != NULL && node->key != key ) {
    prvNode = node;
    node = node->next;
  }

  if ( node == NULL ) {
    // Then the key doesnt exist
    item_t *newNode = count_malloc(sizeof(item_t));
    newNode->count = 1;
    newNode->key = key;
    newNode->next = NULL;
    
    newNode->prev = prvNode;
    prvNode->next = newNode;
  }else{
    // The key exists!
    node->count = node->count + 1;
  }
  return;
}
 
int counters_get(counters_t *ctrs, int key) {
  if (ctrs == NULL) {
    return 0;
  }
  // Get the node with the given key
  item_t *node = ctrs->first;
  while( node != NULL && node->key != key ) node = node->next;

  // It wasnt found
  if ( node == NULL ) {
    return 0;
  }

  // It was! Return it
  return node->count;
}

void counters_delete(counters_t *ctrs) {
  if (ctrs == NULL) {
    return;
  }

  // Get the last node
  item_t *node = ctrs->first;
  while (node != NULL && node->next != NULL) node = node->next;

  while ( node != NULL) {
    // Store the previous
    item_t *prev = node->prev;
    
    // Delete the node
    count_free(node);

    // Move node to prev
    node = prev;
  }

  count_free(ctrs);
}

void counters_set(counters_t *ctrs, int key, int count) {
  if (ctrs == NULL) {
    return;
  }
  // Get the node with the given key
  item_t *node = ctrs->first;

  while( node != NULL && node->key != key )
    node = node->next;
    
  // set count of node to desired value
  if ( node != NULL ) 
    node->count = count;
}


void counters_iterate(counters_t* ctrs, void (*itemfunc)(void *arg, int key, int count), void* arg) {
  if (ctrs == NULL) {
    return;
  }

  item_t* node = ctrs->first;
  
  while( node != NULL ) {
    (*itemfunc)(arg, node->key, node->count);
    node = node->next;
  }
}
