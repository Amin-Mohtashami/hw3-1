#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "parse.h"
#include "io.h"

/*          Get tokens from a line of characters */
/* Return:  new array of pointers to tokens */
/* Effects: token separators in line are replaced with NULL */
/* Storage: Resulting token array points into original line */

#define TOK_SEPARATOR_DEFAULT " \n:"

tok_t *getToks(char *line, int *tok_num, tok_t tok_special[]) {
    int i,j = 0;
    char *c;

    tok_t *toks = malloc(MAXTOKS * sizeof(tok_t));

    for (int i = 0; i < MAXTOKS; i++) {
        toks[i] = NULL; /* empty token array */
        tok_special[i] = NULL; /* empty token special array */
    }

    c = strtok(line, TOK_SEPARATOR_DEFAULT); /* Start tokenizer on line */
    for (i = 0; c && (i < MAXTOKS); i++) {
        if ((c[0] == '>' || c[0] == '<')) {
            tok_special[j] = c;
            j++;
            tok_special[j] = strtok(NULL, TOK_SEPARATOR_DEFAULT);
            j++;
        }
        else
            toks[i] = c;

        c = strtok(NULL, TOK_SEPARATOR_DEFAULT); /* scan for next token */
    }

    *tok_num = i;

    return toks;
}

void freeToks(tok_t *toks) {
  free(toks);
}

void fprintTok(FILE *ofile, tok_t *t) {
    for (int i = 0; i < MAXTOKS && t[i]; i++)
        fprintf(ofile,"%s ", t[i]);
    fprintf(ofile,"\n");
}

/* Locate special processing character */
int isDirectTok(tok_t *t, char *R) {
    for (int i = 0; i < MAXTOKS - 1 && t[i]; i++)
        if (strncmp(t[i],R,1) == 0) return i;
    return 0;
}
