/* A header file for index.c module for writing and reading index files
 *
 *
 * John Kotz + Andrew Laub, May 2016
 */

#include <stdbool.h>

#ifndef __INDEX_H
#define __INDEX_H

// ------Types-------
typedef struct index index_t;


// -----Functions----

/**
 * Creates a new emtpy index
 */
index_t *index_new(const int num_slots);

/**
 * Insert a word into the index, with a given docID
 * If word does not yet exist, creates a counter and adds docID
 * If word does exists and docID does not, then adds docID to counter
 * If both exist, increments counter
 */
bool index_insert(index_t *index, const char *word, const int docID);

/**
 * Puts the index into a given index file
 */
void index_print(index_t *index, FILE *file);

/**
 * Loads the index from a given index file
 */
index_t *index_load(FILE *file, const int num_slots);

/**
 * Delete the index
 */
void index_delete(index_t *index);

/**
 * Finds the word and returns TBD
 * returns NULL if it could not be found
 */
counters_t *index_find(index_t *index, char *word);

#endif // __HASHTABLE_H
