#ifndef __INDEXHELP_H
#define __INDEXHELP_H


/**************** functions ****************/

/*************index_save()*************/
/* Saves an index hashtable to the file index_file in the format
 * 	word docID count [docID count] ...
 * For any number of [docID count] pairs
 */
bool index_save(hashtable_t *ht, FILE *index_file);

/*************index_read()*************/
/* Reads the file index_file in the format
 * 	word docID count [docID count] ...
 * For any number of [docID count] pairs into the specified hashtable.
 */
bool index_read(hashtable_t *ht, FILE *in_file);

#endif // __INDEXHELP_H