# CS50 Lab 4: The Crawler

## Compilation Instructions:
Type **_make_** in the tse directory to compile the crawler.

----
### Usage:
     ./crawler seedURL pageDirectory maxDepth

Where the seedURL is the URL to begin the crawl with, the page directory is the directory to store the html documents in, and maxDepth is the maximum depth that the crawler should pull URLs from.

----
### Exit Status Codes:
* exit 0 --Success.

* exit 1 --Invalid number of arguments.
* exit 2 --Invalid seed URL.
* exit 3 --Photo page directory doesn't exist or isn't writable.
* exit 4 --Invalid max depth.

----

### Assumptions:
* It is assumed that the crawler will not crawl outside the domain: [http://old-www.cs.dartmouth.edu](http://old-www.cs.dartmouth.edu).This implies that the seed page must be internal to that URL.
* The depth of the crawl must be between 0 and 10. Where a depth of 0 would just crawl the seed page.
* The pageDirectory exists and is writable. If this is not the case, the crawler will quit out with an error message(See exit status codes).

----

### Limitations:
> This crawler is only designed to crawl pages internal to Dartmouth's old computer science page. Additionally, this crawler is only designed make small crawls(to a maximum depth of 10) and therefore isn't suited for building a sufficient index to query from even if it were to be used outside the specified URL. 
