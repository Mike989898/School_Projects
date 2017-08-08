# CS50 Lab5: The Indexer

### Compilation instructions:
Type **_make_** in the tse directory to make the whole TSE project, including the indexer.

----

### Usage:
     ./indexer pageDirectory indexFilename

----

### Assumptions:
* pageDirectory has files named 1, 2, 3, ..., without gaps.
* The content of files in pageDirectory follow the format as defined in the crawler specifcations.

----

### Limitations:
> This indexer builds an index on counts of words found in specifc documents. It lacks any understanding of the English language,
and is not able to contextualize words, find roots, or do any sort of word processing(it takes words literally).