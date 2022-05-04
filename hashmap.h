//
//  hashmap.h
//  hashmap
//
//  Created by XuNing on 2021/10/31.
//

#ifndef hashmap_h
#define hashmap_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct hashmap hashmap;

/**
 * Creates a new hash map. Returns NULL if memory allocation fails.
 *
 * @param initial_capacity number of expected entries
 * @param hash function which hashes keys. If use a string type key, just pass
 * `NULL` to use the default hash algorithm.
 * @param is_keys_equal function which compares keys for equality. If use a
 * string type key, just pass `NULL` to use the default compare function.
 */
hashmap *hashmap_create(size_t initial_capacity, int (*hash)(void *key),
                        bool (*is_keys_equal)(void *key1, void *key2));

/**
 * Frees entries in the hash map. Does not free the keys or values themselves.
 */
void hashmap_destroy(hashmap *map);

/**
 * Gets a value from the map.
 *
 * @return NULL if no entry for the given key is found or if the value itself is
 * NULL.
 */
void *hashmap_get(hashmap *map, void *key);

/**
 * Sets value for the given key in the map.
 *
 * @return pre-existing value if any. If memory allocation fails, this function
 * returns NULL, the map's size does not increase, and errno is set to ENOMEM.
 */
void *hashmap_set(hashmap *map, void *key, void *value);

/**
 * Returns the number of entries in the map.
 */
size_t hashmap_count(hashmap *map);

/**
 * Removes an entry from the map.
 *
 * @return the removed value or NULL if no entry was present.
 */
void *hashmap_remove(hashmap *map, void *key);

/**
 * Invokes the given callback on each entry in the map. Stops iterating if
 * the callback returns false.
 */
void hashmap_iterate(hashmap *map,
                     bool (*callback)(void *key, void *value, void *context),
                     void *context);

/**
 * Locks the hash map so only the current thread can access it.
 */
void hashmap_lock(hashmap *map);

/**
 * Unlocks the hash map so other threads can access it.
 */
void hashmap_unlock(hashmap *map);

#ifdef __cplusplus
}
#endif

#endif /* hashmap_h */
