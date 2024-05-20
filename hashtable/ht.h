//
//  ht.h
//  hashtable
//
//  Created by Andrey Marchenko on 05.05.24.
//

#ifndef ht_h
#define ht_h

#include <stdio.h>

typedef struct ht ht;

// Create hash table and return pointer to it, or NULL if out of memory.
ht* ht_create;

// Free memory allocated for hash table, including allocated keys.
void ht_destroy(ht* table);

// Get item with given key (NULL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
void* ht_get(ht* table, const char* key);

#endif /* ht_h */
