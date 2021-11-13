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

typedef struct hashmap hashmap;

hashmap *hashmap_create(size_t capacity, int (*hash)(void *key),
                        bool (*is_keys_equal)(void *key1, void *key2));

void hashmap_free(hashmap *map);

void *hashmap_get(hashmap *map, void *key);

void *hashmap_set(hashmap *map, void *key, void *value);

size_t hashmap_count(hashmap *map);

void *hashmap_remove(hashmap *map, void *key);

void hashmap_iterate(hashmap *map, bool (*callback)(void *key, void *value));

void hashmap_lock(hashmap *map);
void hashmap_unlock(hashmap *map);

#ifdef __cplusplus
}
#endif

#endif /* hashmap_h */
