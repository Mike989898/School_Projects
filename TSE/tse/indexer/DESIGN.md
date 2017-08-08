# TSE Indexer Requirements Spec
The TSE indexer is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. Its companion, the index tester, loads an index file produced by the indexer and saves it to another file.

The indexer **shall**:

1. execute from a command line with usage syntax
    * `./indexer pageDirectory indexFilename`
	* where `pageDirectory` is a directory produced by the crawler, and
	* where indexFilename is the name of a file into which the index should be written.
2. read documents from the `pageDirectory`, each of which has a unique document ID,
	* wherein the document id starts at 1 and increments for each new document,
	* and the filename is of form `pageDirectory/id`,
	* and the first line of the file is the URL,
	* and the second line of the file is the depth,
	* and the rest of the file is the page content.
3. build an inverted-index data structure mapping from words to (documentID, count) pairs, wherein each count represents the number of occurrences of the given word in the given document. Ignore words with fewer than three characters, and NormalizeWord before indexing.
4. create a file indexFilename and write the index to that file, in the format described below.

The indexer **shall** validate its command-line arguments:

* `pageDirectory` is a directory produced by the crawler (see below), and
* `indexFilename` is the name of a writeable file.
The indexer *may* assume that

* `pageDirectory` has files named 1, 2, 3, …, without gaps.
* The content of files in `pageDirectory` follow the format as defined in the crawler specs; thus your code (to create a `WebPage` by reading a file) need not have extensive error checking.
The index tester shall:

1. execute from a command line with usage syntax
	* `./indextest oldIndexFilename newIndexFilename`
	* where `oldIndexFilename` is the name of a file produced by the indexer, and
	* where `newIndexFilename` is the name of a file into which the index should be written.
2. load the index from the `oldIndexFilename` into an inverted-index data structure.
3. create a file `newIndexFilename` and write the index to that file, in the format described below.
It need not validate its command-line arguments other than to ensure that it receives precisely two arguments; it may simply try to open the oldIndexFilename for reading and, later, try to open the newIndexFilename for writing. You may use `indextest.sh` (provided) to verify that the output is identical to (or equivalent to) the input.

The index tester *may* assume that

* The content of the index file follows the format specified below; thus your code (to recreate an index structure by reading a file) need not have extensive error checking.

# TSE Indexer Design SPEC

## Indexer
The heart of the `indexer` is an inverted index, which for each word tells you what files contain this word and how many times for each file. This is created as a `hashtable` with a key for each word which maps to a `counter` where every file is the key to the count of how many times the word appears in the file.

We also create two helper functions `index_save` and `index_read` in a seperate file called indexhelper.c in the libs fold which respectively write an index in the format given by Prof. Kotz to the second argument of the indexer, and read in from a file into a hashtable. We seperated them into a different file because this way it was easier for `indexer` and `indextest` to use these methods, and also the querier will also need it. 

The `indexer` does the following
1. Process and validate each parameter
2. Initialize the `hashtable` data structure
3. Read through each file
	1. Convert the contents of the file into a string.
	2. Run the provided `GetNextWord` on the file repeatedly, adding that file to every counter associated with each word.
4. It then saves this using `index_save`.
5. FInally it cleans up by deleting each hashtable.

## Indextest

Indextest is very simple, since index_read and index_save are both in the seperate `libs/indexhelper.c` file. So it performs the following.
1. Process and validate command-line parameters
2. Initialize data structure index
3. `index_read(file1, index)`
4. `index_save(file2, index)`
5. clean up data structures

# TSE Indexer Implementation SPEC

##Indexer 

To do this we have several functions:
1. `parseArguments` which reads and validates the arguments.
2. `index_build` which initializes the hashtable, reads in each file, adds its contents to the hashtable, and closes each file.
3. `convertFileContentsToString` which takes in each file and writes its contents to one long string. Note we don't save this to a webpage, because we don't really need the url or the depth in this case, so it makes more sense to just read it as a string.

##Indextest

There is almost nothing here. The majority of the code is argument validation in the `main` function. there is also a `cleanup` function which closes the files and deletes the hashtable, and a `deleteCounters` function which we pass as the deleting function to the hashtable.

# TSE Indexer Testing SPEC

##Indexer 

We will test this on the results of crawler at depths 1 and 3, and then look at the output. For the simple case of the depth 1 test, we will manually check several files to make sure the result seems alright, by running `ctrl F` several times and making sure for a small sample of words that they actually are there.

We will check the `index_save` and `index_read` functions using the provided `indextest.sh` file.