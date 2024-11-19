#ifndef _WEBCACHE_H_
#define _WEBCACHE_H_

// Individual hash table entry
typedef struct Cache_Entry {
    char *path;   // Endpoint path--key to the cache
    char *content_type;
    int content_length;
    void *content;

    struct Cache_Entry *prev, *next; // Doubly-linked list
} Cache_Entry;

// A cache
typedef struct Cache {
    struct hashtable *index;
    Cache_Entry *head, *tail; // Doubly-linked list
    int max_size; // Maxiumum number of entries
    int cur_size; // Current number of entries
} Cache;

extern Cache_Entry *alloc_entry(char *path, char *content_type, void *content, int content_length);
extern void free_entry(Cache_Entry *entry);
extern Cache *cache_create(int max_size, int hashsize);
extern void cache_free(Cache *cache);
extern void cache_put(Cache *cache, char *path, char *content_type, void *content, int content_length);
extern Cache_Entry *cache_get(Cache *cache, char *path);

#endif