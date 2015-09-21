#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_THREADS 4
#define MAXWORD     1024
#define DEBUG       0

typedef struct dict {
	char *word;
	int count;
	struct dict *next;
} dict_t;

// globals
dict_t* d;
pthread_mutex_t guardian;
pthread_mutexattr_t guardian_attr;
FILE *infile, *outfile;

void
dbg(char* _dbg_text) {
	if (DEBUG) fprintf(outfile, "%s", _dbg_text);
}

char*
make_word( char *word ) {
	return strcpy(malloc(strlen(word)+1), word);
}

dict_t*
make_dict(char *word) {
	dict_t *nd = (dict_t *) malloc(sizeof(dict_t));

	nd->word = make_word(word);
	nd->count = 1;
	nd->next = NULL;
	return nd;
}

dict_t*
insert_word( dict_t *d, char *word ) {
	//   Insert word into dict or increment count if already there
	//   return pointer to the updated dict

	dict_t *nd;
	dict_t *pd = NULL;		// prior to insertion point
	dict_t *di = d;		// following insertion point

	// Search down list to find if present or point of insertion
	while(di && ( strcmp(word, di->word ) >= 0) ) {
		if ( strcmp( word, di->word ) == 0 ) {
      		di->count++;		// increment count
			return d;			// return head
    	}

    	pd = di;			// advance ptr pair
    	di = di->next;
  	}

	nd = make_dict(word);		// not found, make entry
	nd->next = di;		// entry bigger than word or tail

	if (pd) {
    	pd->next = nd;
		return d;			// insert beond head
  	}

	return nd;
}

void
print_dict() {
	while (d) {
		printf("[%d] %s\n", d->count, d->word);
		d = d->next;
	}
}

void*
thread_worker(void* rank) {
	int words = 1, inword, c;
	long _self = (long) rank;
	char* _wb = malloc(sizeof(char) * (MAXWORD+1));

	while (words) {
		inword = 0;

		pthread_mutex_lock(&guardian);
		c = fgetc(infile);
		dbg("[1] pthread_mutex_lock -\n");
		while (c != EOF && inword < MAXWORD) {
			if (inword && !isalpha(c)) {
				_wb[inword] = '\0'; // terminate the word string
				break;
			}

			if (isalpha(c)) _wb[inword++] = c;
			c = fgetc(infile);
		}

		if (DEBUG) fprintf(outfile, "thread_num: %ld - %s\n", _self, _wb);
		pthread_mutex_unlock(&guardian);

		dbg("[1] pthread_mutex_unlock -\n\n");

		if (c == EOF) break;

		pthread_mutex_lock(&guardian);
		dbg("[2] pthread_mutex_lock -\n");

		d = insert_word(d, _wb);              // add to dict

		pthread_mutex_unlock(&guardian);
		dbg("[2] pthread_mutex_unlock -\n\n");
	}

	free(_wb);
	return NULL;
}

void
words() {
	pthread_mutexattr_init(&guardian_attr);
	/* use a recursive mutex lock */
	pthread_mutexattr_settype(&guardian_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&guardian, &guardian_attr);

	pthread_t* thread_bank = malloc(sizeof(pthread_t) * NUM_THREADS);

	for (int i = 1; i <= NUM_THREADS; i++) {
		if (pthread_create(&thread_bank[i-1], NULL, thread_worker, (void*) (intptr_t) i) != 0)
			printf("Error: pthread_create - could not create thread!\n");
	}

	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread_bank[i], NULL);

	free(thread_bank);

	if (pthread_mutex_destroy(&guardian) != 0)
		printf("Error: pthread_mutex_destroy - could not destroy mutex!\n");
}

int
main(int argc, char *argv[]) {
	infile = stdin;
	outfile = stdout;

	// file stuff
	if (argc >= 2)
		infile = fopen (argv[1],"r");

	if(!infile) {
		printf("Unable to open %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}

	words(); // actual program logic
	print_dict();     // print results

	fclose(infile);
	fclose(outfile);
}
