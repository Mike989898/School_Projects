# TSE Querier Requirements Spec
The TSE querier is a standalone program that reads the index file produced by the TSE indexer, and answers search queries submitted via stdin.

The querier **shall**:

1. execute from a command line with usage syntax
    * `./querier pageDirectory indexFilename`
    * where `pageDirectory` is a directory produced by the crawler, and
    * where `indexFilename` is an index file produced by the indexer.
2. load the index from `indexFilename` into an internal data structure.

3. read search queries from stdin, one per line, until EOF.
    1. clean and parse each query according to the syntax described below.
    2. print the ‘clean’ query for user to see.
    3. use the index to identify the set of documents that satisfy the query, as described below.
    4. if no documents satisfy the query, print `No documents match.`
    5. otherwise, rank the resulting set of documents according to its score, as described below, and print the set of documents in decreasing rank order; for each, list the score, document ID and URL. (Obtain the URL by reading the first line of the relevant document file from the `pageDirectory.`)
The **querier** shall validate its command-line arguments:

`pageDirectory` is a directory produced by the crawler, and
`indexFilename` is the name of a readable file.
The querier may assume that the input directory and files follow the designated formats.

`pageDirectory` has files named 1, 2, 3, …, without gaps.
The content of document files in `pageDirectory` follow the format as defined in the crawler specs; thus your code (to read a document file) need not have extensive error checking.
The content of the file named by `indexFilename` follows our Index file format; thus your code (to recreate an index structure by reading a file) need not have extensive error checking.

# TSE Querier Design SPEC

## Querier

### User Interface
The Command Line.

### Inputs and Outputs 
    * Arguments will be checked for size
    * That `pageDirectory` is a valid directory.
    * and we can write to `indexFilename`.
    * The output will be in the form:
    	`DocID, score, url`
    Where DocID is the number of the ID in the index, score is the associated score, and url is the string url associated with the DocID.
    * If an invalid query is given there will be the neccesarry error message.

### Modules

We will use the previous `lib` and `common` modules, with the code in a file called `querier.c` in the `querier` directory.

The major functions will be:
* `parseArguments` which will parse and verify the given arguments.
* `performQuery` which runs through every query and passes it to
* `parseQuery` which checks and validates if a query is valid and passes the query to
* `printQuery` which then prints the query.
* `index_read` which reads the index.
* `orCounters` which runs the or on the parsed query, and calls 
* `andCounters` which does the and parsing repeatedly before returning to or.
* `unionCounters` which unions two counters.
* `intersectCounters` which intersects two counters.
* `printQueryResult` which sorts and prints the result of the query.

The psuedocode is:
`
parseArguments():
performQuery():
	index_read()
	while (there's another line from stdin)
		parseQuery()
		printQuery()
		orCounter()`

###  Data Structures
* A `hashtable of counters` to store the index
* A struct for a `query` which is an aray of `char *` and the `int` size.
* A struct for the `running_count` with a `counter` and the current position we have parsed though the query.
* Struct for passing several `counters` to the intersection
* Struct for the result of a `counter` with a key:value.
* An Array of Count Pair with the attached size.

###  Testing
We'll use the index we got from last time, and also the fuzzquerier. Manually check a few entries and then run the fuzzquerier with and without valgrind.

# TSE Querier Implementation SPEC

### Function prototypes
* Parses and validates arguments. Called from main:
	`static bool parseArguments(const int argc, char *argv[])`
	* Checks that there are the correct number of arguments.
	* That both the index Directory and index file are present

* Reads from StdIn, and then performs the query. Calls `parseQuery`
	`static bool performQuery()`

* Parses a query fed to it from `performQuery`.

	`static query_t *parseQuery(char *query)`

	* Checks to make sure that the query is in the proper format
	* Checks that the query has only alphabetic words
* Creates a new Query and initializes it empty.
	`static query_t *queryNew()`
* Creates and deletes `Query`s.
	`static void queryDelete(int size, query_t *query)`
* Write the validated query to `stdout`.
	`static void printQuery(query_t *query)`

* Goes through the given query, and does the ors.
	`static running_count_t 
		*orCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable)`
	* Calls `andCounters` to go through every set of 'ands'.
	* Unions the results of each 'and'
* Takes a query, and runs through it, going through every 'and' statement.
	`static counters_t 
		*andCounters(query_t *query, running_count_t *running_count, hashtable_t *hashtable)`
		* Gets results from the hashtable.
		* Intersects counters as needed.
		* If it finds a NULL result, always returns NULL.
* unions two counters, stores the result in the first.
	`static void unionCounters(counters_t *running_counter, counters_t *counters2)`
* intersects two counters, returns the intersection (different then union).
	`static counters_t 
		*intersectCounters(counters_t *running_count_counters, counters_t *counters_to_iterate)`

* Initializes a new `RunningCount`.
	`static running_count_t *runningCountNew()`
* Frees a `RunningCount` as needed.
	`static void runningCountDelete(running_count_t *running_count)`

* Creates a new `bundle` of counters to pass to the intersection.
	`static counters_bundle_t *countersBundleNew()`

* Prints and sorts the result of a query.
	`static void printQueryResult(counters_t *counters)`
	* Converts the result of a query (stored as a `counter`) into a `count_pair_array`
	* sorts using `qsort`
	* Then prints the result to `stdin`.

* Opens and closes a file to get the url given a doc_id.
	`static char* getDocUrl(int doc_id)`

* Checks that every element of a query is just alphabetic characters.
	`static bool isAlphabetic(char *word)`
* Create and delete a `count_pair_array` respectively.
	`static count_pair_array_t *newCounterPairArray(int size)`
	`static void deleteCountPairArray(count_pair_array_t *count_pair_array)`

The helper functions to be passsed to data structures and iterators are not included here, as they're pretty simple.

###  Data Structures
* The `hashtable` and `counter` are as before.
* `struct query{
	int num_words; //size of query in number of words
	char** word_array; //array of words from the query input
}`
*  To store the latest results from our query.
`struct running_count{
	int position; //position in the query
	counters_t *counters; //the current counter results from ANDing and ORing words in the query
}`
* Stores counters to pass to intersection. `counters1` and `counters2` are the two things to be intersected.
`counters_bundle{
	bool was_match; // for keeping track of if an intersection or union occured
	counters_t *counters1;
	counters_t *counters2;
}`
* Stores the results of a count pair to be used in `count_pair_array`
`struct count_pair {
	int key;
	int count;
} `
* Array to be sorted. We need position since this is passed iteratively to the count.
` struct count_pair_array {
	int size;
	int position;
	count_pair_t **array;
}`

## Testing Plan
We will do several manual checks on a small index we have to make sure the output is correct. We also check those in valgrind to find memory leaks. 

We will then use fuzzquery to generate a large number of queries to feed in using one of Prof. Kotz's given indexes and run that a huge number of times