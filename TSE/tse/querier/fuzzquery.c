/* 
 * fuzzquery - generate a series of random queries for testing 
 * 
 * usage: 
 *   fuzzquery indexFile numQueries randomSeed
 *
 * David Kotz, May 2016
 */

#include <stdio.h>
#include <stdlib.h> // random, srandom
#include <string.h> // strchr
#include <ctype.h> // isdigit
#include <stdbool.h>
#include "../lib/cs50ds.h"
#include "../common/common.h"

/**************** file-local global variables ****************/
static char *program;
static const char *UnixDictionary = "/usr/share/dict/words";

/**************** file-local global types ****************/
typedef struct {
  int nwords;		      // number of words in the list
  char **words;		      // array of words
} wordlist_t;

/**************** local functions ****************/
static void parse_args(const int argc, char *argv[], 
		       char **indexFilename, int *numQueries, int *randomSeed);
static int parse_int(const char *arg, const char *what);
static wordlist_t *wordlist_load(const char *indexFilename);
static void wordlist_delete(wordlist_t *words);
static void generate_query(const wordlist_t *wordlist, 
			   const wordlist_t *dictionary);
static const void *checkp(const void *p, const char *message);

/**************** main ****************/
int
main(const int argc, char *argv[])
{
  char *indexFilename;
  int numQueries;
  int randomSeed;

  // parse the arguments
  // function exits if any errors in parsing
  parse_args(argc, argv, &indexFilename, &numQueries, &randomSeed);

  // seed random number generator
  srandom(randomSeed);

  // load an array full of words from the given index file
  wordlist_t *wordlist = wordlist_load(indexFilename);
  if (wordlist == NULL) {
    fprintf(stderr, "%s cannot load words from index file '%s'\n",
	    program, indexFilename);
    exit(9);
  }
  
  // load an array full of words from the Unix dictionary
  wordlist_t *dictionary = wordlist_load(UnixDictionary);
  if (dictionary == NULL) {
    fprintf(stderr, "%s cannot load words from dictionary '%s'\n",
	    program, UnixDictionary);
    exit(9);
  }
  
  fprintf(stderr, "%s: generating %d queries from %d words\n", 
	  program, numQueries, wordlist->nwords);

  // generate random queries, using that wordlist
  for (int q = 0; q < numQueries; q++)
    generate_query(wordlist, dictionary);

  wordlist_delete(wordlist);
  wordlist_delete(dictionary);

  // count_report(stderr, "end of main");
}

/**************** parse_args ****************/
/* Parse the command-line arguments, filling in the parameters;
 * if any error, print to stderr and exit.
 */
static void
parse_args(const int argc, char *argv[], 
	   char **indexFilename, int *numQueries, int *randomSeed)
{
  /**** usage ****/
  program = argv[0];
  if (argc != 4) {
    fprintf(stderr, "usage: %s indexFile numQueries randomSeed\n", program);
    exit(1);
  }

  /**** indexFile ****/
  *indexFilename = argv[1];

  /**** numQueries ****/
  *numQueries = parse_int(argv[2], "numQueries");

  /**** randomSeed ****/
  *randomSeed = parse_int(argv[3], "randomSeed");

}

/**************** parse_int ****************/
/* parse a positive integer argument.
 * if any error, print to stderr, and exit.
 */
static int
parse_int(const char *arg, const char *what)
{
  // verify that the argument string has only digits
  for (const char *p = arg; *p != '\0'; p++) {
    // the string must all be digits
    if (!isdigit(*p)) {
      fprintf(stderr, "usage: %s: invalid %s '%s'\n", program, what, arg);
      exit (2);
    }
  }

  // convert to an integer, now that we know it's all digits
  return atoi(arg);
}

/**************** wordlist_load ****************/
/* load the set of words in the given index file,
 * saving them in a freshly-allocated array;
 * the caller must later call words_delete() on the result.
 */
static wordlist_t *
wordlist_load(const char *indexFilename)
{
  checkp(indexFilename, "wordlist_load indexFilename");

  FILE *fp = fopen(indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s: cannot open index file '%s'\n", 
	    program, indexFilename);
    return NULL;
  }

  // How many words will we have to read?
  int nwords = lines_in_file(fp);

  if (nwords == 0) {
    fprintf(stderr, "%s: index file '%s' has no words\n",
	    program, indexFilename);
    fclose(fp);
    return NULL;
  }

  // Make array to hold the words
  char **words = count_calloc_assert(nwords, sizeof(char*), "words[]");

  // wrap the result in a wordlist
  wordlist_t *wordlist = count_malloc_assert(sizeof(wordlist_t), "wordlist");
  wordlist->nwords = nwords;
  wordlist->words = words;

  // read in all the words  
  for (int w = 0; w < nwords; w++) {
    char *word = readline(fp);
    if (word == NULL) {
      // this should not happen - but clean up if it does.
      fprintf(stderr, "%s: index file '%s' ends unexpectedly\n",
	      program, indexFilename);
      wordlist->nwords = w;
      wordlist_delete(wordlist);
      fclose(fp);
      return NULL;
    } else {
      // truncate and save the word
      char *space = strchr(word, ' ');
      if (space != NULL)
	*space = '\0';
      words[w] = word;
    }
  }
  fclose(fp);

  return wordlist;
}

/**************** wordlist_delete ****************/
/* free the memory allocated by wordlist_load
 */
static void
wordlist_delete(wordlist_t *wordlist)
{
  checkp(wordlist, "wordlist_delete words");

  char **words = wordlist->words;
  int nwords = wordlist->nwords;
  for (int w = 0; w < nwords; w++)
    free(words[w]); // was allocated by readline()

  count_free(wordlist->words);
  count_free(wordlist);
}

/**************** generate_query ****************/
/* generate one random query and print to stdout.
 * pull random words from the wordlist and from the dictionary.
 */
static void
generate_query(const wordlist_t *wordlist, const wordlist_t *dictionary)
{
  // some parameters that affect query generation
  const int max_words = 6;	      // generate 1..max_words
  const float or_probability = 0.3;   // P(OR between two words)
  const float and_probability = 0.2;  // P(AND between two words)
  const float dict_probability = 0.0; // P(draw from dict instead of wordlist)

  int qwords = random() % max_words + 1; // number of words in query
  for (int qw = 0; qw < qwords; qw++) {
    // draw a word either dictionary or wordlist
    if ((random() % 100) < (dict_probability * 100)) {
      printf("%s ", dictionary->words[random() % dictionary->nwords]);
    } else {
      printf("%s ", wordlist->words[random() % wordlist->nwords]);
    }

    // last word?
    if (qw < qwords-1) {
      // which operator to print?
      int op = random() % 100;
      if (op < (and_probability * 100))
	printf("AND ");
      else if (op < (and_probability * 100 + or_probability * 100))
	printf("OR ");
      else 
	; // nothing (implicit AND)
    }
  }
  printf("\n");
}

/**************** checkp ****************/
/* if pointer p is NULL, print error message and die,
 * otherwise, return p unchanged.
 */
static const void *
checkp(const void *p, const char *message)
{
  if (p == NULL) {
    fprintf(stderr, "CHECKP: %s\n", message);
    exit (99);
  }
  return p;
}
