#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "../lib/cs50ds.h"
#include "../common/common.h"

/**************** global types ****************/
typedef struct query{
	int num_words; //size of query in number of words
	char** word_array; //array of words from the query input
} query_t;

typedef struct running_count{
	int position; //position in the query
	counters_t *counters; //the current counter results from ANDing and ORing words in the query
}running_count_t;

typedef struct counters_bundle{
	bool was_match; // for keeping track of if an intersection or union occured
	counters_t *counters1;
	counters_t *counters2;
}counters_bundle_t;

typedef struct count_pair {
	int key;
	int count;
} count_pair_t;

typedef struct count_pair_array {
	int size;
	int position;
	count_pair_t **array;
} count_pair_array_t;

/*************Internal Function Prototypes*************/
static bool parseArguments(const int argc, char *argv[]);
static query_t *queryNew();
static void queryDelete(int size, query_t *query);
static query_t *parseQuery(char *query);
static bool performQuery();
static void printQuery(query_t *query);
static counters_t *andCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable);
static running_count_t *orCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable);
static void unionCounters(counters_t *running_counter, counters_t *counters2);
static counters_t *intersectCounters(counters_t *running_count_counters, counters_t *counters_to_iterate);
static void intersectCountersCopyHelper(void *arg, int key, int count);
static void intersectCountersCompareHelper(void *arg, int key, int count);
static running_count_t *runningCountNew();
static void runningCountDelete(running_count_t *running_count);
static counters_bundle_t *countersBundleNew();
void deleteCounters(void *data);
static void printQueryResult(counters_t *counters);
void printOutCounters(void *arg, int key, int count);
static char* getDocUrl(int doc_id);
static bool isAlphabetic(char *word);
static void unionCountersHelper(void *arg, int key, int count);
static void counterSize(void *arg, int key, int count);
static void counterToArrayHelper(void *arg, int key, int count);
static count_pair_array_t *newCounterPairArray(int size);
static int compareCountPair(const void *c1, const void *c2);
static void printCountPairArray(count_pair_array_t *count_pair_array);
static void deleteCountPairArray(count_pair_array_t *count_pair_array);

/********Globals************/
char *photo_page_dir; 
FILE *index_file;

/*************main()*************/
int main(const int argc, char *argv[]){

	if (!parseArguments(argc, argv)){
		return 1;//invalid arguments
	}

	if (!performQuery()){
		printf("Query could not be performed properly");
		return 2;//invalid query
	}
}

/*************parseArguments()*************/
/* Parses the command line arguments.
 */
static bool parseArguments(const int argc, char *argv[]){
	if(argc != 3){
		printf("Inproper number of arguments. Please input: pageDirectory indexFilename\n");
		return false;
	}

	//check that the photopage directory given is a valid one
	//start by building a file path to check if .crawler exists
	//and is readable.
	photo_page_dir = argv[1];
	char *full_file_path = malloc(sizeof(char *)*(strlen(photo_page_dir) + strlen("/.crawler") + 1));
	if (full_file_path == NULL) {
		return false;
	}
	strcpy(full_file_path, photo_page_dir);//copy pageDirectory so as to not change contents
	strncat(full_file_path, "/.crawler", strlen("/.crawler"));

	//check the status of the .crawler file
	FILE *read_file;
	if((read_file=(fopen(full_file_path, "r"))) == NULL){
		printf("Could not find %s", full_file_path);
		free(full_file_path);
		printf(". Invalid pageDirectory!\n");
		return false;
	}
	else{
		free(full_file_path);
		fclose(read_file);
	}

	//finally, check that we can read from the index
	if((index_file=(fopen(argv[2], "r"))) == NULL){
		printf("Cannot create file %s\n", argv[2]);
		return false;

	}
	//all of the conditions were met for valid input
	return true;
}

/*************performQuery()*************/
/* Checks to see if a query is valid, then runs it.
 */
static bool performQuery(){
	char *input;
	query_t *query;

	//build the index from the index file
	hashtable_t *hashtable = hashtable_new(2000, deleteCounters);
	if (hashtable == NULL) {
		return false;
	}

	if (!index_read(hashtable, index_file)){
		return false; // couldn't read the index
	}

	while((input = readline(stdin)) != NULL) {
		if ((query = parseQuery(input)) == NULL){
			free(input);
			continue; // query couldn't be parsed
		}
		else{
			printQuery(query);
			running_count_t *running_count = runningCountNew();
			if (running_count == NULL) {
				return false;
			}

			if((running_count = (orCounters(query, running_count, hashtable))) == NULL){
				printf("No documents match.\n");
				runningCountDelete(running_count);
				continue;
			}
			printQueryResult(running_count->counters);
			runningCountDelete(running_count);
		}

		queryDelete(query->num_words, query);
		free(input);
	}

	hashtable_delete(hashtable);
	fclose(index_file);

	return true;
}

/*************andCounters()*************/
/* Runs through an 'and' expression, returns when it hits an 'or' or 
 *the end of the query.
 */
static counters_t *andCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable){
	bool nulled = false;

	running_count_t *this_running_count = running_count;
	counters_t *and_count = NULL;
	while(((this_running_count->position) < (query->num_words))){

		if((strcmp(query->word_array[(this_running_count->position)], "or")) == 0){
				this_running_count->position++;
				return and_count; // go to the next word in query
		}
		else{
			//if we hit an AND, skip over it(since the assumption is that we're always ANDing unless we hit an OR)
			if((strcmp(query->word_array[(this_running_count->position)], "and")) == 0){
				this_running_count->position++;
				continue; // go to the next word in query
			}

			counters_t *count;
			if((count = hashtable_find(hashtable, query->word_array[(this_running_count->position)])) == NULL){
				this_running_count->position++;
				//couldn't find the word in the hash, therefore no documents match the query
				nulled = true;
				continue;
			}

			if (nulled) {
				this_running_count->position++;
				continue;
			}
			//intersect the counters sets
			
			// TODO:
			// MEM LEAK ?
			if((and_count = intersectCounters(and_count, count)) == NULL) {
				this_running_count->position++;
				nulled = true;
				counters_delete(and_count);
				continue;
			}
			this_running_count->position++;

			if (nulled){
				counters_delete(and_count);
			}
		}
	}
	//reached the end of the query, return the result
	if (nulled) {
		return NULL;
	} else {
		return and_count;
	}
}

/*************orCounters()*************/
/* While there are things left in the query, parses them and passes
 * groups to the andCounters() function, get's the result back and
 * then unions the result.
 */
static running_count_t *orCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable){
	if (running_count->counters == NULL) {
		running_count->counters = counters_new();	
		if (running_count->counters == NULL) {
			return NULL;
		}
	}

	while ((running_count -> position) < (query->num_words)) {
		if((strcmp(query->word_array[(running_count->position)], "or")) == 0) {
			running_count -> position++;
			continue;

		} else {
			counters_t *and_count = andCounters(query, running_count, hashtable);
			unionCounters(running_count->counters, and_count);
			counters_delete(and_count);
		}
	}

	return running_count;
}

/*************unionCounters()*************/
/* union two counters, store the result in the first.
 */
static void unionCounters(counters_t *running_counter, counters_t *counters2){
	if (counters2 != NULL) {
		counters_iterate(counters2, unionCountersHelper, running_counter);
	}


	return;
}

/*************unionCounters()*************/
/* Helper function to be passed to the counters_iterate to union two
 * couters.
 */
static void unionCountersHelper(void *arg, int key, int count) {
	counters_t *running_counters = (counters_t *) arg;

	int result = counters_get(running_counters, key);
	counters_set(running_counters, key, count + result);
}

/*************intersectCounters()*************/
/* Intersects two counters stores and returns the result in a new counter.
 */
static counters_t 
*intersectCounters(counters_t *running_count_counters,
	counters_t *counters_to_iterate){
	//copy the contents of the first found counter into
	//a new counter so that we have a counter we can manipulate
	//without affecting the counters inside the hash table
	counters_t *new_counters; 

	if(running_count_counters == NULL){
		new_counters = counters_new();

		if (new_counters == NULL) {
			return false;
		}
		counters_iterate(counters_to_iterate, intersectCountersCopyHelper, new_counters);
		return new_counters;
	}
	else{
		//create a bundle of counters to pass to the iterator function
		counters_bundle_t *counters_bundle;
		if ((counters_bundle = countersBundleNew()) == NULL){
			return NULL;
		}
		//fill the bundle with a new counter and the running_count_counter
		counters_bundle->was_match = false;
		counters_bundle->counters1 = counters_new();
		if (counters_bundle->counters1 == NULL) {
			return NULL;
		}

		counters_bundle->counters2 = running_count_counters;
		counters_iterate(counters_to_iterate, intersectCountersCompareHelper, counters_bundle);
		new_counters = counters_bundle->counters1;
		//free the bundle and the old running count
		if(!(counters_bundle->was_match)){
			counters_delete(counters_bundle->counters1);
			counters_delete(counters_bundle->counters2);
			free(counters_bundle);
			return NULL;//no documents match
		}
		counters_delete(counters_bundle->counters2);
		free(counters_bundle);
		return new_counters;
	}
}

/*************intersectCountersCopyHelper()*************/
/*
 *Function pointer to be passed to the iterator
 *this function is explicitly for copying the contents of a counter
 *into a new counter.
 */
static void intersectCountersCopyHelper(void *arg, int key, int count){
	counters_t *counters = (counters_t *) arg;
	counters_set(counters, key, count);
}


/*************intersectCountersCompareHelper()*************/
/*
 *Function pointer to be passed to the iterator
 *this function is explicitly for comparing the keys
 *and values of the counters_t being iterated through
 *and the counters_t passed to it via the bundle.
 *the intersect result is then put in a new counters_t
 *object
 * 
 */
static void intersectCountersCompareHelper(void *arg, int key, int count){
	counters_bundle_t *counters_bundle = (counters_bundle_t *) arg;
	counters_t *new_counters = counters_bundle->counters1;
	counters_t *running_counters = counters_bundle->counters2;
	int running_count;
	if((running_count = counters_get(running_counters, key)) !=0){
		//count exits for that document key
		//set the count to the minimum of the two counts
		counters_bundle->was_match = true;
		if(running_count < count){
			counters_add(new_counters, key);
			counters_set(new_counters, key, running_count);
		}
		else{
			counters_add(new_counters, key);
			counters_set(new_counters, key, count);
		}
	}
}

/*************parseQuery()*************/
/* Parses and validates a query
 */
static query_t *parseQuery(char *input){
	char *word;
	query_t *query;
	bool AND_or_OR_allowed_next = false;
	int size;

	size = (strlen(input) + 1)/2; //maximum number of words in the input

	//get the first word
	if ((word = strtok(input, " \t\r\n\v\f")) == NULL){
		//couldn't get the first word
		return NULL;
	}
	else{
		//check all the characters in the word are alphabetic
		if(!isAlphabetic(word)){
			printf("Error: Words with non-alphabetic characters are not allowed.\n");
			return NULL;
		}

		//lowercase the word
		word = NormalizeWord(word);
		//check the first word isn't an AND or OR
		if((strcmp(word, "and")) == 0 || (strcmp(word, "or")) == 0){
			printf("Error: AND or OR statement not allowed at the start of a query.\n");
			return NULL;
		}
		else{
			AND_or_OR_allowed_next = true;
		}

		//initialize the query
		query = queryNew(size);

		if (query == NULL) {
			return NULL;
		}

		//put the first word into the query words array
		query->word_array[0] = malloc(strlen(word) + 1);
		if (query->word_array[0] == NULL) {
			return NULL;
		}
		strcpy(query->word_array[0], word);
		//add each word from the input to the query words array
		int i = 1;//start from 1 since we already added the first word
		while((word = strtok(NULL, " \t\r\n\v\f")) != NULL){
			//check all the characters in the word are alphabetic
			if(!isAlphabetic(word)){
				printf("Error: Words with non-alphabetic characters are not allowed.\n");
				return NULL;
			}
			//lower case word
			word = NormalizeWord(word);

			//check if the word is AND or OR
			if((strcmp(word, "and")) == 0 || (strcmp(word, "or")) == 0){
				//check to see if the AND or OR was allowed
				if(!AND_or_OR_allowed_next){
					//it wasn't allowed, so free everything we've allocated and return NULL
					printf("Error: Multiple ANDs or ORs not allowed in a row\n");
					//pass queryDelete the counter,i, to free all the words we've allocated
					queryDelete(i, query);
					return NULL;
				}
				AND_or_OR_allowed_next = false;
			}
			else{
				//if an AND or OR wasn't found, we allow one for the next word
				AND_or_OR_allowed_next = true;
			}

			query->word_array[i] = malloc(strlen(word) +1);
			if (query->word_array[i] == NULL) {
				return NULL;
			}
			//copy the new word into the arrow
			strcpy(query->word_array[i], word);
			i++;
		}
		//assign the word count
		query->num_words = i;
		//check the last word wasn't an AND or an OR
		if(!AND_or_OR_allowed_next){
			printf("Error: Cannot end query with AND or OR statement\n");
			queryDelete(query->num_words, query);
			return NULL;
		}
		return query; 
	}
}

/*************queryNew()*************/
static query_t *queryNew(int size){
	query_t *query = malloc(sizeof(query_t));
	if(query == NULL){
		return NULL; //not enough space for the query
	}
	else{
		if((query->word_array = malloc(size*sizeof(char *))) == NULL){
			return NULL; //not enough space for the array of character pointers
		}
		return query;
		}
}

/*************runningCountNew()*************/
static running_count_t *runningCountNew(){
	running_count_t *running_count = malloc(sizeof(running_count_t));
	if(running_count == NULL){
		return NULL;
	}
	//intialize struct members
	running_count->position = 0;
	running_count->counters = NULL;
	return running_count;
}

static void runningCountDelete(running_count_t *running_count) {
	counters_delete(running_count->counters);
	free(running_count);
}

/*************countersBundleNew()*************/
static counters_bundle_t *countersBundleNew(){
	counters_bundle_t *counters_bundle = malloc(sizeof(counters_bundle_t));
	if(counters_bundle == NULL){
		return NULL;
	}
	//intialize struct memebers
	counters_bundle->counters1 = NULL;
	counters_bundle->counters2 = NULL;
	return counters_bundle;
}

/*************queryDelete()*************/
/* Deletes a query.
 */
static void queryDelete(int size, query_t *query){
	//free all the individualy words up to the specified size
	int i;
	for(i=0; i<size; i++){
		free(query->word_array[i]);
	}
	//free the word array
	free(query->word_array);

	//free the query
	free(query);
	query = NULL;
}

/*************deleteCounters()*************/
/* Helper function for the hashtable to delete the hashtable.
 */
void deleteCounters(void *data){
	counters_t *counter_to_delete = (counters_t *) data;
	counters_delete(counter_to_delete);
}

/*************printQuery()*************/
/* Given a valid query, print it out.
 */
static void printQuery(query_t *query){
	int i;
	printf("Query: ");
	for(i=0; i<(query->num_words); i++){
		printf("%s ", query->word_array[i]);
	}
	printf("\n");
}

/*************printQuery()*************/
/* Given the results of a query, sorts and prints it.
 */
static void printQueryResult(counters_t *counters){
	if (counters == NULL) {
		printf("No documents match.\n");
		return;
	}

	int size = 0;
	counters_iterate(counters, counterSize, &size);

	if (size == 0) {
		printf("No documents match.\n");
		return;
	}

	count_pair_array_t *array = newCounterPairArray(size);
	if (array == NULL) {
		return;
	}
	counters_iterate(counters, counterToArrayHelper, array);
	qsort(array->array, array->size, sizeof(count_pair_t), compareCountPair);

	printCountPairArray(array);
	deleteCountPairArray(array);
}

/*************printOutCounters()*************/
/* Prints out the given counter in the given format.
 */
void printOutCounters(void *arg, int key, int count){
	char *url;
	if ((url= getDocUrl(key)) ==NULL){
		url = "None";
	}
	printf("DocID:%5d  |  Score:%5d  |  url:%s\n", key, count, url);
	free(url);
}

/*************counterSize()*************/
/* This is a helper function to find the size of a counter for 
 * initializing an array.
 */
static void counterSize(void *arg, int key, int count) {
	int *size = (int *) arg;
	(*size)++;
}

/*************counterToArrayHelper()*************/
/* Function to be passed to a counters_iterate to add the element
 * to a given counters_pair_array.
 */
static void counterToArrayHelper(void *arg, int key, int count) {
	count_pair_array_t *array = (count_pair_array_t*) arg;
	count_pair_t *count_pair = malloc(sizeof(count_pair));
	if(count_pair == NULL){
		return;
	}
	//intialize struct memebers
	count_pair->key = key;
	count_pair->count = count;

	array->array[array->position] = count_pair;
	array->position++;
}

/*************newCounterPairArray()*************/
/* Creates a new counter_pair_array, which is an array of counter_pairs with
 * an associated size.
 */
static count_pair_array_t* newCounterPairArray(int size) {
	count_pair_array_t *array = malloc(sizeof(struct count_pair_array));
	if (array == NULL) {
		return NULL;
	}

	array->size = size;
	array->array = malloc(sizeof(count_pair_t *) * size);
	if (array->array == NULL) {
		return NULL;
	}
	array->position = 0;

	return array;
}

/*************compareCountPair()*************/
/* Function to be passed to qsort to sort an array *count_pair_t
 * by the count.
 */
static int compareCountPair(const void *c1, const void *c2) {
	//Remember the array I'm storing is an array of **count_pair_t,
	// so this needs to be double cast.
	count_pair_t **cp1 = (count_pair_t **)c1;
	count_pair_t **cp2 = (count_pair_t **)c2;
	return ((*cp1)->count < (*cp2)->count);
}

/*************printCountPairArray()*************/
/* Prints a count_pair_array
 */
static void printCountPairArray(count_pair_array_t *count_pair_array) {
	count_pair_t **array = count_pair_array->array;
	for (int i = 0; i < count_pair_array->size; i++) {
		count_pair_t *count_pair = array[i];
		int key = count_pair->key;
		char *url;
		if ((url= getDocUrl(key)) == NULL){
			url = "None";
		}
		printf("DocID:%5d  |  Score:%5d  |  url:%s\n", key, count_pair->count, url);
		free(url);
	}
}

/*************deleteCountPairArray()*************/
/* Deletes a count_pair_array
 */
static void deleteCountPairArray(count_pair_array_t *count_pair_array) {
	count_pair_t **array = count_pair_array->array;
	for (int i = 0; i < count_pair_array->size; i++) {
		count_pair_t *count_pair = array[i];
		free(count_pair);
	}
	free(array);
	free(count_pair_array);
}


/*************getDocUrl()*************/
/* Goes through the index folder and finds the right ID.
 */
static char* getDocUrl(int doc_id){
	char counter_string_buffer[6]; //this could potentially need to be bigger depending on how many pages we add
	sprintf(counter_string_buffer, "%d", doc_id);
	char *temp_file_name;
	if ((temp_file_name = malloc(sizeof(char *)*(strlen(photo_page_dir) + 1))) == NULL){
		return NULL; // not enough spece for the temp_file_name
	}
	//build file path
	strcpy(temp_file_name, photo_page_dir);
	strncat(temp_file_name, "/", strlen("/"));
	strncat(temp_file_name, counter_string_buffer, strlen(counter_string_buffer));

	FILE *next_file;
	if(!((next_file=(fopen(temp_file_name, "r"))) == NULL)){
		//return the first line of the document
		char* result = readline(next_file);
		fclose(next_file);
		free(temp_file_name);
		return result;
	}
	else{
		return NULL;//wasn't able to read from the crawler document
	}
}

/*************isAlphabetic()*************/
static bool isAlphabetic(char *word){
	int i;
	for(i=0; i<strlen(word); i++){
		if(!isalpha(word[i])){
			return false;//non aplhabetic character found
		}
	}
	return true;
}
