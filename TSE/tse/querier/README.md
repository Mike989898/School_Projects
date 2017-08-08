# CS50 Lab5: The Querier

### Compilation instructions:
Type **_make_** in the tse directory to make the whole TSE project, including the querier.

----

### Usage:
     ./querier pageDirectory indexFilename

----

### Assumptions:
* pageDirectory has files named 1, 2, 3, ..., without gaps.
* The content of files in pageDirectory follow the format as defined in the crawler specifcations.
* The contents of the index follow the guidlines defined in the specifications for the indexer.
* After running the querier, the user will be able to enter queries until they hit controll + D.

----

### Limitations:
> Our implementation of the querier only implements AND precedence, and doesn't support
paranthesis around expressions as a form a precedence. Additionally, our querier does not
ignore duplicate inputs. If an or is used between two inputs, the score will be double for
the list of documents containing that word(potentially unfairly weighting that word). Moreoever,
our querier uses a primative ranking(or scoring) scheme, where an AND sequence produces the minimum
score of the intersection, and an OR squence produces a summed score of the union. If we were going
to use this querier as part of an actual search engine, we would devise a more complex ranking scheme
so that it could not be 'gamed' as easily. 