#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apt_hashtable.h"
#include "apt_cache.h"

/**
 * Allocate a cache entry
 */
Cache_Entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    // implemented
    Cache_Entry* entry = (Cache_Entry*)malloc(sizeof(struct Cache_Entry));
    entry->path = strdup(path);
    entry->content_type = strdup(content_type);
    entry->content = malloc(content_length);
    memcpy(entry->content, content, content_length);
    entry->content_length = content_length;

    entry->prev = NULL;
    entry->next = NULL;
    
    return entry;
}

/**
 * Deallocate a cache entry
 */
void free_entry(Cache_Entry *entry)
{
    // implemented
    free(entry->path);
    free(entry->content);
    free(entry->content_type);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(Cache *cache, Cache_Entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(Cache *cache, Cache_Entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
Cache_Entry *dllist_remove_tail(Cache *cache)
{
    Cache_Entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
Cache *cache_create(int max_size, int hashsize)
{
    // implemented
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->index = hashtable_create(hashsize, NULL);
    cache->head = NULL;
    cache->tail = NULL;
    cache->max_size = max_size;
    cache->cur_size = 0;

    return cache;
}

void cache_free(Cache *cache)
{
    Cache_Entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        Cache_Entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(Cache *cache, char *path, char *content_type, void *content, int content_length)
{
    // implemented
    Cache_Entry* newEntry = alloc_entry(path, content_type, content, content_length);

    dllist_insert_head(cache, newEntry);
    hashtable_put(cache->index, path, newEntry);
    cache->cur_size++;

    if(cache->cur_size > cache->max_size){
        Cache_Entry* old_entry = dllist_remove_tail(cache);
        hashtable_delete(cache->index, old_entry->path);
        free(old_entry);
    }
}

/**
 * Retrieve an entry from the cache
 */
Cache_Entry *cache_get(Cache *cache, char *path)
{
    // implemented
    Cache_Entry* entry = hashtable_get(cache->index, path);
    if(entry)
        dllist_move_to_head(cache, entry);

    return entry;
}
