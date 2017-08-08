# CS50: The TSE Project

## Contributers: Akira Murphy and Michael Kilgore _(With lots of help from Professor Kotz)_

### Compilation Instructions:
To compile the entire tse project, type _**make**_ in the tse directory.

### Modules
The main modules are:
* `common`: A set of files given to us by Prof. Kotz to help us to read the web, files, read through files.
* `lib` The files from Lab3, with `hashtable`, `list`, `bag`, and `counters`, along with Kotz's `memory` helper.
* `crawler` the web crawler which creates a set of files containing the output of crawled web pages.
* `indexer` which runs through the files created by the crawler and finds which words go where.
* `querier` which when run and given a query finds which files contain a set of words. 

`.
├── README.md
├── querier
│   ├── TESTING.md
│   ├── README.md
│   ├── querier.c
│   ├── Makefile
│   ├── fuzzquery.c
│   ├── DESIGN.md
├── Makefile
├── lib
│   ├── memory
│   │   ├── memory.h
│   │   ├── memory.c
│   │   └── Makefile
│   ├── Makefile
│   ├── list
│   │   ├── Makefile
│   │   ├── listtest.c
│   │   ├── list.h
│   │   └── list.c
│   ├── indexhelp
│   │   ├── Makefile
│   │   ├── indexhelp.h
│   │   └── indexhelp.c
│   ├── hashtable
│   │   ├── Makefile
│   │   ├── list.h
│   │   ├── list.c
│   │   ├── hashtabletest.c
│   │   ├── hashtable.h
│   │   └── hashtable.c
│   ├── cs50ds.h
│   ├── counters
│   │   ├── Makefile
│   │   ├── countertest
│   │   ├── counterstest.c
│   │   ├── counters.h
│   │   └── counters.c
│   └── bag
│       ├── Makefile
│       ├── bagtest.c
│       ├── bag.h
│       └── bag.c
├── indexer
│   ├── testscript.sh
│   ├── TESTING.md
│   ├── README.md
│   ├── myIndex
│   ├── Makefile
│   ├── indextest.sh
│   ├── indextest.c
│   ├── indexsort.sh
│   ├── indexsort.awk
│   ├── indexer.c
│   ├── expected_outputs
│   │   ├── too_many_args.txt
│   │   ├── sorted_index.txt
│   │   ├── no_args.txt
│   │   ├── inv_dir.txt
│   │   └── index.txt
│   ├── DESIGN.md
│   └── bash_index_test.sh
├── crawler
│   ├── testscript.sh
│   ├── TESTING.md
│   ├── testDir
│   ├── README.md
│   ├── out.txt
│   ├── Makefile
│   └── crawler.c
└── common
    ├── word.h
    ├── word.c
    ├── web.h
    ├── web.c
    ├── Makefile
    ├── file.h
    ├── file.c
    └── common.h`

### Usage:
See each of the individual module's README.md file for usage instructions.

### A Bit About the Project:
The goal of this project is to design a tiny search engine(TSE) that operates within the old
dartmouth computer science website domain. The project contains three modules:
the crawler, the indexer, and the querier. The crawler fetches pages up to a specified depth starting from a 
seed URL, and writes the pages out to a page directory. The indexer then reads these files and
builds an index that contains the word followed by a list of documents with the occurence frequency
of that word in that particular document. The querier loads the index produced by the indexer
and answers queries Backus-Naur Form queries provided by a user. The querier returns the results of the query
in the form: DOCUMENTID, SCORE, URL(in sorted order by score). _For more information on each module, see the appropriate
README.md file(located in each module's directory)._