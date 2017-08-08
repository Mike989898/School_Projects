/* 
 * memory - count_malloc and related functions 
 * 
 * see memory.h for file and function documentation.
 *
 * David Kotz, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

/**************** file-local global variables ****************/
// track malloc and free across *all* calls within this program.
static int nmalloc = 0;	   // number of successful malloc calls
static int nfree = 0;	   // number of free calls
static int nfreenull = 0;  // number of free(NULL) calls


/**************** assertp ****************/
void *
assertp(void *p, char *message)
{
  if (p == NULL) {
    fprintf(stderr, "MALLOC FAILED: %s\n", message);
    exit (99);
  }
  return p;
}

/**************** count_malloc() ****************/
void *
count_malloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr != NULL)
    nmalloc++;
  return ptr;
}

/**************** count_malloc_assert() ****************/
void *
count_malloc_assert(size_t size, char *message)
{
  void *ptr = assertp(malloc(size), message);
  nmalloc++;
  return ptr;
}


/**************** count_calloc() ****************/
void *
count_calloc(size_t nmemb, size_t size)
{
  void *ptr = calloc(nmemb, size);
  if (ptr != NULL)
    nmalloc++;
  return ptr;
}

/**************** count_calloc_assert() ****************/
void *
count_calloc_assert(size_t nmemb, size_t size, char *message)
{
  void *ptr = assertp(calloc(nmemb, size), message);
  nmalloc++;
  return ptr;
}


/**************** count_free() ****************/
void 
count_free(void *ptr)
{
  if (ptr != NULL) {
    free(ptr);
    nfree++;
  } else {
    // it's an error to call free(NULL)!
    nfreenull++;
  }
}

/**************** count_report() ****************/
void 
count_report(FILE *fp, char *message)
{
  fprintf(fp, "%s: %d malloc, %d free, %d free(NULL), %d net\n", 
	  message, nmalloc, nfree, nfreenull, nmalloc - nfree - nfreenull);
}
