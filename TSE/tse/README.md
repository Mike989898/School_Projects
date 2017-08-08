# CS50: The TSE Project

## Contributers: Akira Murphy and Michael Kilgore 

### Compilation Instructions:
To compile the entire tse project, type _**make**_ in the tse directory.

### Modules
The main modules are:
* `common`: A set of files given to us by Prof. Kotz to help us to read the web, files, read through files.
* `lib` The files from Lab3, with `hashtable`, `list`, `bag`, and `counters`, along with Kotz's `memory` helper.
* `crawler` the web crawler which creates a set of files containing the output of crawled web pages.
* `indexer` which runs through the files created by the crawler and finds which words go where.
* `querier` which when run and given a query finds which files contain a set of words. 

```
tse
├── Makefile
├── README.md
├── common
│   ├── Makefile
│   ├── common.h
│   ├── file.c
│   ├── file.h
│   ├── web.c
│   ├── web.h
│   ├── word.c
│   └── word.h
├── crawler
│   ├── Makefile
│   ├── README.md
│   ├── TESTING.md
│   ├── crawler.c
│   └── testscript.sh
├── indexer
│   ├── DESIGN.md
│   ├── Makefile
│   ├── README.md
│   ├── TESTING.md
│   ├── expected_outputs
│   │   ├── index.txt
│   │   ├── inv_dir.txt
│   │   ├── no_args.txt
│   │   ├── sorted_index.txt
│   │   └── too_many_args.txt
│   ├── indexer.c
│   ├── indexsort.awk
│   ├── indexsort.sh
│   ├── indextest.c
│   ├── indextest.sh
│   ├── myIndex
│   └── testscript.sh
├── lib
│   ├── Makefile
│   ├── bag
│   │   ├── Makefile
│   │   ├── bag.c
│   │   ├── bag.h
│   │   ├── bagtest.c
│   │   └── core.8687
│   ├── counters
│   │   ├── Makefile
│   │   ├── counters.c
│   │   ├── counters.h
│   │   └── counterstest.c
│   ├── cs50ds.h
│   ├── hashtable
│   │   ├── Makefile
│   │   ├── hashtable.c
│   │   ├── hashtable.h
│   │   ├── hashtabletest.c
│   │   ├── list.c
│   │   └── list.h
│   ├── indexhelp
│   │   ├── Makefile
│   │   ├── indexhelp.c
│   │   └── indexhelp.h
│   ├── list
│   │   ├── Makefile
│   │   ├── list.c
│   │   ├── list.h
│   │   └── listtest.c
│   └── memory
│       ├── Makefile
│       ├── memory.c
│       └── memory.h
└── querier
    ├── DESIGN.md
    ├── Makefile
    ├── README.md
    ├── TESTING.md
    ├── fuzzquery.c
    └── querier.c
```

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