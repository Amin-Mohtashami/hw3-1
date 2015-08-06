#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

/* typedefs */
typedef unsigned long counter_t;

/* Method declarations */
void wc ( FILE *infile, char *inname);
void print ( counter_t lines, counter_t words, counter_t bytes, char *fname);
bool isallowed ( char c);

/* Globals vars */
counter_t glob_lines = 0, glob_words = 0, glob_bytes = 0;

int
main (int argc, char *argv[]) {
    char c;

    if (argc > 1) {
        int i;

        for (i = 1; i < argc; i++)
            wc(fopen(argv[i], "r"), argv[i]);

        if (argc > 2) {
            print(glob_lines, glob_words, glob_bytes, "total");
        }
    }
    else
        while (c != '\0')
            scanf("%c", &c);

    return 0;
}

void
wc( FILE *infile, char *inname) {
    if (infile == NULL) {
        fprintf(stderr, "wc: %s: No such file or directory\n", inname);
        return;
    }

    int c;
    counter_t lines = 0, words = 0, bytes = -1;

    while (!feof(infile)) {
        c = fgetc(infile);
        if (c == '\n') lines++;
        if (c == ' ') words++;
        bytes++;
    }

    print(lines, words, bytes, inname);

    glob_lines += lines;
    glob_words += words;
    glob_bytes += bytes;

    fclose(infile);
}

bool
isallowed( char c) {
    return isalpha(c);
}

void
print( counter_t lines, counter_t words, counter_t bytes, char *fname) {
    printf(" %lu %lu %lu %s\n", lines, (words+lines), bytes, fname);
}
