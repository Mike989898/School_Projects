/*
 * word-related functions for extracting and cleaning words from html
 *
 * See "word.h" for full documentation.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word.h"

/**************** GetNextWord ****************/
/*
 * GetNextWord - returns the next word from doc[pos] into word
 * See "word.h" for full documentation.
 * Code is courtesy of Ray Jenkins and/or Charles Palmer, 
 *   cleaned by David Kotz in April 2016.
 */

int 
GetNextWord(const char* doc, int pos, char **word)
{
  const char *beg;                         // beginning of word
  const char *end;                         // end of word

  // make sure we have something to search, and a place for the result
  if (doc == NULL || word == NULL) {
    return -1;
  }

  // consume any non-alphabetic characters
  while (doc[pos] != '\0' && !isalpha(doc[pos])) {
    // if we find a tag, i.e., <...tag...>, skip it
    if (doc[pos] == '<') {
      end = strchr(&doc[pos], '>');    // find the close
      
      if (end == '\0' || (++end) == '\0') { // ran out of html
	return -1;
      }

      pos = end - doc;	      // skip over the <...tag...>
    } else {
      pos++;		      // just move forward
    }
  }

  // ran out of html
  if (doc[pos] == '\0') {
    *word = NULL;
    return -1;
  }

  // pos is at the first character of a word
  beg = &(doc[pos]);

  // consume word
  while (doc[pos] != '\0' && isalpha(doc[pos])) {
    pos++;
  }
  // at this point, doc[pos] is the first character *after* the word.

  // pos refers to the last character of the word
  end = &(doc[pos-1]);

  // 'beg' points to first character of the word
  // 'end' points to last character of the word
  int wordlen = end - beg + 1;

  // allocate space for length of new word + '\0'
  *word = calloc(wordlen + 1, sizeof(char));
  if (*word == NULL) {	      // out of memory!
    return -1;
  }

  // copy the new word
  strncpy(*word, beg, wordlen);

  return pos;
}

/**************** NormalizeWord ****************/
/*
 * NormalizeWord - convert word to canonical representation.
 * In this trivial implementation, we convert to lower case;
 * one could imagine converting more sophisticated transformations
 * (like converting plural to singular, removing prefix or suffix).
 *
 * See "word.h" for full documentation.
 * Code by David Kotz, April 2016.
 */
char *
NormalizeWord (char *word)
{
  if (word != NULL) {
    for (char *p = word; *p != '\0'; p++)
      *p = tolower(*p);
  }
  return word;
}

