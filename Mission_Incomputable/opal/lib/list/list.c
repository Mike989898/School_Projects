/*
 * list.c - a list module
 *
 * John Kotz + Andrew Laub, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../memory/memory.h"
#include "list.h"

// ======Types======

// Local
typedef struct node {
  struct node* next;
  char* key;
  void* data;
} node_t;

// Global
typedef struct list {
  node_t* first;
} list_t;

// =====Functions======
list_t *list_new(void) {
  list_t* list = malloc(sizeof(list_t));

  if (list != NULL) {
    list->first = NULL;
  }

  // Could be NULL, but thats what I want to return then
  return list;
}

bool list_insert(list_t *list, char *key, void *data) {
  if (list == NULL || key == NULL) {
    return false;
  }

  node_t *prvNode = NULL;
  node_t *node = list->first;

  while ( node != NULL && strcmp(node->key, key) != 0 ) {
    prvNode = node;
    node = node->next;
  }
  
  if ( node == NULL ) {
    // If this is the case, then we made it through the whole list and did not find it
    node_t *newNode = malloc(sizeof(node_t));

    if ( newNode != NULL ) {
      // Copy the key
      // Allocate the space
      newNode->key = malloc(strlen(key) + 1);
      
      if (newNode->key != NULL) {
	// Copy into it
	strcpy(newNode->key, key);

	// Store the data
	newNode->data = data;
	newNode->next = NULL;
	// Set up the next
	if (prvNode == NULL) {
	  // The list is empty
	  list->first = newNode;
	}else{
	  prvNode->next = newNode;
	}
	return true;
      }else{
	free(newNode);
	return false;
      }
    }
  }
  return false;
}

void *list_find(list_t *list, char *key) {
  if (list == NULL || key == NULL) {
    return false;
  }

  node_t *node = list->first;
  
  // Goes through to the end or to the node with the given key
  while ( node != NULL && strcmp(node->key, key) != 0 ) {
    node = node->next;
  }

  if ( node != NULL ) {
    // We found it!
    return node->data;
  }

  return NULL;
}

void list_delete(list_t *list) {
  if (list == NULL) {
    return;
  }

  while (list->first != NULL) {
    node_t *node = list->first;
    list->first = node->next;

    free(node->key);
    free(node);
  }

  free(list);
}

void list_iterate(list_t *list, void (*itemfunc)(void *arg, char* key, void* data), void *arg) {
  if (list == NULL) {
    return;
  }

  node_t *node = list->first;
  
  while (node != NULL) {
    (*itemfunc)(arg, node->key, node->data);
    node = node->next;
  }
}
