
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../hashtable/hashtable.h"
#include "../counters/counters.h"

void printDocAndWordCount(void *arg, char *key, void *data );
void printCounter (void *arg, int key, int count);

/*************index_read()*************/
bool index_save(hashtable_t *ht, FILE *index_file) {
	hashtable_iterate(ht, printDocAndWordCount, index_file);
	return true;
}

/*************index_read()*************/
bool index_read(hashtable_t *ht, FILE *in_file) {
	char word[500];
	int key = 0;
	int count = 0;

	while(fscanf(in_file, "%s ", word) > 0) {
		counters_t *word_counter = counters_new();
		while (fscanf(in_file, "%d %d ", &key, &count) > 0) {
			counters_set(word_counter, key, count);
		}
		hashtable_insert(ht, word, word_counter);
	}
	return true;
}

/*************printDocAndWordCount()*************/
/* Function that is passed to the hashtable to be used to print each
 * entry of the hashtable.
 */
void printDocAndWordCount(void *arg, char *key, void *data ){
	FILE *fp = (FILE*) arg;
	fprintf(fp, "%s", key);
	counters_t *cntr = (counters_t *)data;
	counters_iterate(cntr, printCounter, fp);
	fprintf(fp, "\n");

}

/*************printDocAndWordCount()*************/
/* Function passed to the counter to be used by the counter to print to a
 * a file.
 */
void printCounter (void *arg, int key, int count) {
	FILE *fp = (FILE*) arg;
	fprintf(fp, " %d %d", key, count);
}
