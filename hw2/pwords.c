#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_THREADS 16
#define MAXWORD     1024

typedef struct dict {
	char *word;
	int count;
	struct dict *next;
} dict_t;

// globals
dict_t* d;
pthread_mutex_t guardian = PTHREAD_MUTEX_INITIALIZER;

char* make_word( char *word ) {
	return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t* make_dict(char *word) {
	dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
	
	nd->word = make_word( word );
	nd->count = 1;
	nd->next = NULL;
	return nd;
}

dict_t* insert_word( dict_t *d, char *word ) {
	//   Insert word into dict or increment count if already there
	//   return pointer to the updated dict

	dict_t *nd;
	dict_t *pd = NULL;		// prior to insertion point
	dict_t *di = d;		// following insertion point

	// Search down list to find if present or point of insertion
	while(di && ( strcmp(word, di->word ) >= 0) ) {
		if( strcmp( word, di->word ) == 0 ) {
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

void print_dict() {
	while (d) {
    	printf("[%d] %s\n", d->count, d->word);
    	d = d->next;
	}
}

int get_word( char *buf, int n, FILE *infile) {
	int inword = 0;
	int c;

	while( (c = fgetc(infile)) != EOF && inword < n) {
		if (inword && !isalpha(c)) {
			buf[inword] = '\0';	// terminate the word string
			return 1;
    	}

	    if (isalpha(c)) {
    		buf[inword++] = c;
    	}
	}

	return 0;			// no more words
}

void* thread_worker(void* infile) {
  	char wordbuf[MAXWORD];
	//printf("thread [%ld] - started\n", pthread_self());

  	while( get_word( wordbuf, MAXWORD, infile ) ) {
		pthread_mutex_lock(&guardian);
    	d = insert_word(d, wordbuf); // add to dict
		pthread_mutex_unlock(&guardian);
	}
	
	return NULL;
}

void words(void *infile) {
	pthread_t* thread_bank = malloc(sizeof(pthread_t) * NUM_THREADS);

	for (int i = 0; i < NUM_THREADS; i++) {
		if (pthread_create(&thread_bank[i], NULL, thread_worker, infile) != 0)
			printf("Error: pthread_create - could not create thread!\n");
	}

	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(thread_bank[i], NULL);

	free(thread_bank);	
}

int main( int argc, char *argv[] ) {
  	FILE *infile = stdin;

  	// file stuff
  	if (argc >= 2)
    	infile = fopen (argv[1],"r");

  	if( !infile ) {
    	printf("Unable to open %s\n",argv[1]);
   		exit( EXIT_FAILURE );
  	}

 	words( infile ); // actual program logic
  	print_dict();     // print results

  	fclose( infile );
}
