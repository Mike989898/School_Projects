/* 
 * memory - count_malloc and related functions 
 * 
 * 1. Replacements for malloc(), calloc(), and free(),
 *    that count the number of calls to each,
 *    so you can print reports about the current balance of memory.
 * 
 * 2. Variants that 'assert' the result is non-NULL;
 *    if NULL occurs, kick out an error and die.
 *
 * David Kotz, April 2016
 */

#ifndef __MEMORY_H
#define __MEMORY_H


/**************** assertp ****************/
/* If pointer p is NULL, print error message and die,
 * otherwise, return p unchanged.  Works nicely as a pass-through:
 *   thingp = assertp(malloc(sizeof(thing)), "thing");
 */
void *assertp(void *p, char *message);

/**************** count_malloc_assert() ****************/
/* Like malloc() but track the number of successful allocations,
 * and, if response is NULL, print error and die.
 * Unlike malloc(), it takes a 'message' parameter.
 */
void *count_malloc_assert(size_t size, char *message);

/**************** count_malloc() ****************/
/* Just like malloc() but track the number of successful allocations */
void *count_malloc(size_t size);

/**************** count_calloc() ****************/
/* Just like calloc() but track the number of successful allocations */
void *count_calloc(size_t nmemb, size_t size);

/**************** count_free() ****************/
/* Just like free() but track the number of calls */
void count_free(void *ptr);

/**************** count_report() ****************/
/* report the current malloc/free counts */
void count_report(FILE *fp, char *message);

#endif // __MEMORY_H
