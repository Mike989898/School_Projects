/*
 * word-related functions for extracting and cleaning words from html
 *
 * Code is courtesy of Ray Jenkins and/or Charles Palmer.
 */

#ifndef __WORD_H
#define __WORD_H

/**************** GetNextWord ****************/
/*
 * GetNextWord - returns the next word from doc[pos] into word
 *
 * Assumptions:
 *     1. doc has been allocated
 *     2. don't care about opening/closing tags: ignore anything between <...>
 *     3. if the html is malformed, we don't care: match '<' with next '>'
 *
 * Pseudocode:
 *     1. skip any leading non-alphabetic characters
 *     2. if we find a tag, i.e., <...tag...>, skip that tag
 *     3. save beginning of the word
 *     4. find the end, i.e., first non-alphabetic character
 *     5. create a new word buffer
 *     6. copy the word into the new buffer
 *     7. return first position past end of word
 * 
 * Memory contract:
 *     1. inbound, doc points to an existing string
 *     2. on return, *word points to malloc'd space 
 *                   and the caller is responsible for freeing that space.
 */

int GetNextWord(const char* doc, int pos, char **word);

/**************** NormalizeWord ****************/
/*
 * NormalizeWord - convert word to canonical representation.
 * Returns its argument, for convenience.
 */
char *NormalizeWord (char *word);

#endif // __WORD_H
