/*
 * hashtable.c - a hashtable module
 *
 * John Kotz + Andrew Laub, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../memory/memory.h"
#include "hashtable.h"


// =======Structs=======

// Local

typedef struct node {
  struct node *next;
  char *key;
  void *data;
} node_t;

typedef struct list {
  node_t *first;
} list_t;

// Global

typedef struct hashtable {
  list_t **lists;
  int slots;
  node_t *lastFailedInsert;
} hashtable_t;

// ======Functions======

// Local Prototpyes

static node_t *createNewNode(char *key, void *data);

// Global

/**
 * Hashes the key within this number of slots
 */
int hash(const char* key, int slots) {
  int value = 0;
  for (char* c = (char*)key; *c != '\0'; c++) {
    value += *c;
  }

  return (value) % slots;
}

unsigned long
JenkinsHash(const char *str, unsigned long mod)
{
  if (str == NULL)
    return 0;

  size_t len = strlen(str);
  unsigned long hash, i;

  for (hash = i = 0; i < len; ++i)
    {
      hash += str[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash % mod;
}

hashtable_t *hashtable_new(const int num_slots) {
  hashtable_t *table = count_malloc(sizeof(hashtable_t));

  if ( table != NULL ) {
    table->lists = count_calloc(num_slots, sizeof(list_t*));

    if ( table->lists == NULL ) {
      // Failed to allocate!
      count_free(table);
      return NULL;
    }
    
    table->slots = num_slots;
  }
  
  return table;
}

void *hashtable_find(hashtable_t *ht, char *key) {
  unsigned long h = JenkinsHash(key, ht->slots);
  list_t *list = ht->lists[h];

  if ( list != NULL && list->first != NULL ) {
    node_t *node;
    for (node = list->first; node != NULL && strcmp(node->key, key) != 0; node = node->next);

    if ( node != NULL ) {
      return node->data;
    }
  }

  // If the list here is empty or does not exist,
  // or I got to the end and didn't find the key, the key does not exist
  return NULL;
}

bool hashtable_insert(hashtable_t *ht, char *key, void *data) {
  if (ht == NULL) {
    return false;
  }

  unsigned long h = JenkinsHash(key, ht->slots);
  list_t *list = ht->lists[h];

  if ( list != NULL && list->first != NULL ) {
    node_t *prvNode = NULL;
    node_t *node = list->first;

    while ( node != NULL && strcmp(node->key, key) != 0 ) {
      prvNode = node;
      node = node->next;
    }

    if ( node == NULL ) {
      node_t* newNode = createNewNode(key, data);

      if ( newNode != NULL ) {
        prvNode->next = newNode;
        return true;
      }
    }else{
      ht->lastFailedInsert = node;
    }
  }else{
    if ( list == NULL ){
      // There's no list here, so I'll try to fix it
      list = count_malloc(sizeof(list_t));
      ht->lists[h] = list;
      if ( list == NULL ) {
        // I failed to recover from not finding a list here
        return true;
      }
    }

    // There is nothing in this list
    node_t *newNode = createNewNode(key, data);
    
    if ( newNode != NULL ) {
      list->first = newNode;
      return true;
    }
  }

  return false;
}

void hashtable_delete( hashtable_t *ht, void (*removeFunc)(void *data) ) {
  if (ht == NULL) {
    return;
  }
  
  for (int i = 0; i < ht->slots; i++) {
    list_t *list = ht->lists[i];

    if (list != NULL) {
      // We should only clear it if it exists
      // Now we just count_free the list
      while (list->first != NULL) {
	node_t *item = list->first;
	list->first = item->next;
	if (item->data != NULL) {
	  removeFunc(item->data);
	}
	// Now we just need to free this list and its key
	count_free(item->key);
	count_free(item);
      }

      // We have finished cleaning the list, now we just let go of the list
      count_free(list);
    }
  }
  // All the lists are cleaned, now we get rid of our list of them
  count_free(ht->lists);
  // Since we don't need to free the slots, we only need to free the actual hash table now
  count_free(ht);
}

void hashtable_iterate(hashtable_t *ht, void (*itemfunc)(void *arg, char *key, void *data), void *arg) {
  for (int i = 0; i < ht->slots; i++) {
    list_t *list = ht->lists[i];
    
    if (list != NULL) {
      node_t *node = list->first;

      while (node != NULL) {
	// Now we can call the function
	(*itemfunc)(arg, node->key, node->data);
	node = node->next;
      }
    }
  }
}

void hashtable_merge(hashtable_t *dest, hashtable_t *source, void (*mergeData)(void **dataDest, void **dataSource)) {
  for (int i = 0; i < source->slots; i++) {
    list_t *list = source->lists[i];

    if (list != NULL) {
      node_t *node = list->first;

      while (node != NULL) {
	if (!hashtable_insert(dest, node->key, node->data)) {
	  // If not, we now have a handy hidden variable that tells us the destination node
	  node_t *destNode = dest->lastFailedInsert;
	  dest->lastFailedInsert = NULL;
	  
	  mergeData(&(destNode->data), &(node->data));
	}
	
        node = node->next;
      }
    }
  }
}

// Local

static node_t *createNewNode(char *key, void *data) {
  // There is nothing in this list                                                                 
  node_t *newNode = count_malloc(sizeof(node_t));
  if ( newNode == NULL ) {
    return NULL;
  }
  newNode->next = NULL;

  // Copy in the key                                                                               
  newNode->key = count_malloc(strlen(key) + 1);
  if ( newNode->key == NULL ) {
    // Failed to allocate memmory for the key. We failed to create a node                          
    count_free(newNode);
    return NULL;
  }
  strcpy(newNode->key, key);

  // Save data and report success                                                                  
  newNode->data = data;

  return newNode;
}
