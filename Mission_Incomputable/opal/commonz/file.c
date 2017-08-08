/* 
 * file utilities
 * 
 * David Kotz, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "file.h"

/**************** file2string ****************/
/* 
 * See file.h for documentation.
 * Adapted from code by Ray Jenkins.
 */
char *
file2string(FILE *fp) 
{
  int n, pos;
  char *buffer;
  
  // How big is the file?
  fseek (fp, 0, SEEK_END);
  int len = ftell (fp);
  rewind(fp);

  // allocate buffer big enough for file, plus terminating null char
  buffer = calloc(len + 1, sizeof(char));

  // Try to read the whole file, but fread might return less;
  // thus, we must loop, calling fread, until it returns 0 (EOF).
  pos = 0;
  while ((n = fread((buffer + pos), sizeof(char), len, fp)) > 0) 
    pos += n;

  // did we get the whole file?
  if (pos != len) {
    // read error
    free(buffer);
    return NULL;
  }

  return buffer;
}

/**************** readline ****************/
/* 
 * See file.h for documentation.
 */
char *
readline(FILE *fp)
{
  // allocate buffer that might be big enough for "typical" lines
  int len = 81;
  char *line = calloc(len, sizeof(char));

  // Read characters from file until newline or EOF, 
  // expanding the buffer when needed to hold more.
  int pos = 0;
  char c;
  for (pos = 0; (c = fgetc(fp)) != EOF && c != '\n'; pos++) {
    // We need to save line[pos+1] for the terminating null
    // and line[len-1] is the last usable slot, 
    // so if pos+1 is past that slot, we need to grow the buffer.
    if (pos+1 > len-1)
      line = realloc(line, ++len);
    line[pos] = c;
  }

  if (pos == 0 && c == EOF) {
    // no characters were read and we reached EOF
    free(line);
    return NULL;
  } else {
    // pos characters were read into line[0]..line[pos-1].
    line[pos] = '\0'; // terminate string
    return line;
  }
}
