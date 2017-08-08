/* An index module for reading and writing index files
 * 
 *
 * John Kotz + Andrew Laub, May 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../lib/hashtable/hashtable.h"
#include "../lib/memory/memory.h"
#include "../lib/counters/counters.h"
#include "file.h"
#include "word.h"
#include <math.h>

// -----Types------

typedef struct index {
  hashtable_t *ht;
} index_t;

typedef struct indexSearch {
  const char *key;
  counters_t *counter;
} indexSearch_t;

// -----Functions----

// Prototypes

void printHash(void *arg, char *key, void *data);
void printCounter(void *arg, int key, int count);
void hashRemove(void *data);
void hashSearch(void *arg, char *key, void *data);

// Global

index_t *index_new(const int num_slots) {
  index_t *index = count_malloc(sizeof(index));

  if (index != NULL) {
    index->ht = hashtable_new(num_slots);

    if (index->ht == NULL) {
      count_free(index);
      return NULL;
    }
  }

  return index;
}

bool index_insert(index_t *index, const char *word, const int docID) {
  if (index == NULL) {
    return false;
  }

  bool result = false;
  counters_t *counter = counters_new();
  if (!(result = hashtable_insert(index->ht, (char*)word, counter))) {
    // If the word already exists, then forget our new one, and get the old one            
    counters_delete(counter);
    counter = hashtable_find(index->ht, (char*)word);
  }
  // Now we have the counter that is there                                            

  counters_add(counter, docID);
  return result;
}

void index_print(index_t *index, FILE *file) {
  hashtable_iterate(index->ht, printHash, file);
}

index_t *index_load(FILE *file, const int num_slots) {
  index_t *index = index_new(num_slots);
  char *line;

  while ((line = readline(file)) != NULL) {
    // For each line
    // Get the word
    char *word;
    int pos;
    if ((pos = GetNextWord(line, 0, &word)) < 0) {
      fprintf(stderr, "Failed to find a word in line '%s'\n", line);
      return NULL;
    }

    counters_t *counter = counters_new();
    if (counter == NULL) {
      free(word);
      free(line);
      return NULL;
    }
    hashtable_insert(index->ht, word, counter);
    
    int len;
    for (char *lineLeft = line + pos; *lineLeft != '\0' && strlen(lineLeft) > 4; lineLeft += len) {
      int docID;
      int num;
      sscanf(lineLeft, " %d %d", &docID, &num);

      counters_add(counter, docID);
      counters_set(counter, docID, num);
    
      len = floor(log10(docID)) + 4 + floor(log10(num)); 
    }
    free(word);
    free(line);
  }

  return index;
}

void index_delete(index_t *index) {
  if (index == NULL) {
    return;
  }
  hashtable_delete(index->ht, hashRemove);

  count_free(index);
}

counters_t *index_find(index_t *index, char *word) {
  if (index == NULL || word == NULL) {
    return NULL;
  }
  indexSearch_t *search = count_malloc_assert(sizeof(indexSearch_t), "creating search");
  search->key = word;
  search->counter = NULL;
  hashtable_iterate(index->ht, hashSearch, search);

  counters_t *counter = search->counter;
  count_free(search);
  return counter;
}

// Local

void printHash(void *arg, char *key, void *data) {
  FILE* file = (FILE*)arg;
  counters_t *counter = (counters_t *)data;

  fprintf(file, "%s ", key);
  counters_iterate(counter, printCounter, arg);
  fprintf(file, "\n");
}

void printCounter(void *arg, int key, int count) {
  FILE* file = (FILE*)arg;

  fprintf(file, "%d %d ", key, count);
}

void hashRemove(void *data) {
  counters_t *counter = (counters_t *)data;
  counters_delete(counter);
}

void hashSearch(void *arg, char *key, void *data) {
  indexSearch_t *search = (indexSearch_t *)arg;

  if (key != NULL && search != NULL && search->counter == NULL && search->key != NULL && strcmp(search->key, key) == 0) {
    search->counter = (counters_t *) data;
  }
}
