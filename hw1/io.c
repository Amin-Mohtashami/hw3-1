#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

#include "io.h"

/* Read a line from input file and return as a string object */

#define MAXLINE 1024

char *freadln(FILE *ifile) {
  char line[MAXLINE];		/* allocate line buffer on the stack */
  char *r = NULL;		/* ptr to return string object */
  int len;
  char *s = fgets((char *)line, MAXLINE, ifile);
  if (!s) return s;
  len = strlen(s)+1;
  r = malloc(len);
  strncpy(r,s,len);
  return r;
}

void freeln(char *ln) {
  free(ln);
}

char* concat(int count, ...) {
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    // Allocate memory to concat strings
    char *merged = calloc(sizeof(char),len);
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}
