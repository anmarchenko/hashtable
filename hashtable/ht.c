//
//  ht.c
//  hashtable
//

#include <stdlib.h>

#include "ht.h"

typedef struct {
    const char* key;
    void* value;
} ht_entry;

struct ht {
    ht_entry* entries; // hash slots
    size_t capacity; // size of entries array
    size_t length; // number of entries in the hashtable
};

#define INITIAL_CAPACITY 16 // must not be zero

ht* ht_create(void) {
    // allocate space for struct itself
    ht* table = malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;
    
    // allocate space for entry buckets
    table->entries = calloc(table->capacity, sizeof(ht_entry));
    if (table->entries == NULL) {
        free(table);
        return NULL;
    }
    return table;
}

void ht_destroy(ht* table) {
    // free all keys
    for (int i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
    }
    
    // free entries array
    free(table->entries);
    // free table
    free(table);
}

