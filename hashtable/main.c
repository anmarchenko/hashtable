// Example:
// $ echo 'foo bar the bar bar bar the' | ./demo
// foo 1
// bar 4
// the 2
// 3


#include <stdio.h>
#include <stdlib.h>

#include "ht.h"

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

int main(void) {
    ht* counts = ht_create();
    if (counts == NULL) {
        exit_nomem();
    }
    
    char word[101];
    while(scanf("%100s", word) != EOF) {
        void* value = ht_get(counts, word);
        if (value != NULL) {
            int* pcount = (int*)value;
            (*pcount)++;
            continue;
        }
        
        int* pcount = malloc(sizeof(int));
        if (pcount == NULL) {
            exit_nomem();
        }
        *pcount = 1;
        if (ht_set(counts, word, pcount) == NULL) {
            exit_nomem();
        }
    }
    
    // print words and counts and free them all
    hti it = ht_iterator(counts);
    while(ht_next(&it)) {
        printf("%s %d\n", it.key, *(int*)it.value);
        free(it.value);
    }
    
    // show the number of unique words
    printf("%d\n", (int)ht_length(counts));
    
    ht_destroy(counts);
    return 0;
}
