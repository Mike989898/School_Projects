/* 
 * file utilities
 * 
 * David Kotz, April 2016
 */

#ifndef __FILE_H
#define __FILE_H

#include <stdio.h>

/**************** file2string ****************/
/* 
 * Read entire contents into memory and return pointer to 
 * string containing file content, or NULL on error.
 * Caller must later count_free() the result.
 * After the call, file pointer is at EOF.
 */

char *file2string(FILE *fp);

/**************** readline ****************/
/* 
 * Read the next line of the file, and return a pointer to it;
 * caller must later free() the pointer.
 * The string returned includes NO newline and a terminating null.
 * Returns NULL if EOF reached without reading a line.
 */
char *readline(FILE *fp);

#endif // __FILE_H
