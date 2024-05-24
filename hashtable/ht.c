//
//  ht.c
//  hashtable
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>

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


#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// internal function to set a new entry in the table
static const char* ht_set_entry(ht_entry* entries, size_t capacity, const char* key, void* value, size_t* plength) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        // key already exists, overwrite
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return entries[index].key;
        }
        
        index++;
        if (index >= capacity) {
            index = 0;
        }
    }
    
    // add new key to hashtable
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false; // overflow
    }
    
    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }
    
    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
        }
    }
    
    // free old entries array
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}


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

void* ht_get(ht* table, const char* key) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));
    
    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        // we have to compare strings because of collisions
        if (strcmp(key, table->entries[index].key) == 0) {
            return table->entries[index].value;
        }
        
        // key was not in this slot, continue
        index++;
        if (index >= table->capacity) {
            // wrap at end of the array
            index = 0;
        }
    }

    return NULL;
}

const char* ht_set(ht* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }
    
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }
    
    return ht_set_entry(table->entries, table->capacity, key, value, &table->length);
}

size_t ht_length(ht* table) {
    return table->length;
}

hti ht_iterator(ht* table) {
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti* it) {
    ht* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // found non-empty entry!
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}
