#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../lib/cs50ds.h"
#include "../common/common.h"

/*************Internal Function Prototypes*************/
static void deleteCounters(void *data);
static void cleanup(hashtable_t *ht, FILE *in_file, FILE *out_file);

/*Globals*/

/*************Main()*************/
int main(const int argc, const char *argv[]){
	FILE *in_file; 
	FILE *out_file; 

	if (argc != 3) {
		printf("%s: invalid number of arguments", argv[0]);
		return -1;
	} 

	if ((in_file = fopen(argv[1], "r")) == NULL) {
		printf("%s: Cannot read file %s\n", argv[0], argv[1]);
		return -2;
	}

	if ((out_file = fopen(argv[2], "w")) == NULL) {
		printf("%s: Cannot write to file %s\n", argv[0], argv[2]);
		return -3;
	}

	hashtable_t *ht = hashtable_new(2000, deleteCounters); 

	index_read(ht, in_file);
	index_save(ht, out_file);

	cleanup(ht, in_file, out_file);
}
/*************deleteCounters()*************/
/* Helper function for deleting the hastable.
 */
static void cleanup(hashtable_t *ht, FILE *in_file, FILE *out_file) {
	hashtable_delete(ht);
	fclose(in_file);
	fclose(out_file);
}

/*************deleteCounters()*************/
/* Helper function for deleting the hastable.
 */
static void deleteCounters(void *data){
	counters_t *counter_to_delete = (counters_t *) data;
	counters_delete(counter_to_delete);
}
