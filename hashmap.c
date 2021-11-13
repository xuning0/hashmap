//
//  hashmap.c
//  hashmap
//
//  Created by XuNing on 2021/10/31.
//

#include "hashmap.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct entry entry;
struct entry {
  void *key;
  void *value;
  int hash;
  entry *next;
};

struct hashmap {
  entry **buckets;
  size_t bucket_count;
  size_t size;
  int (*hash)(void *key);
  bool (*is_keys_equal)(void *key1, void *key2);
  pthread_mutex_t lock;
};

static inline int default_hash_key(void *key) {
  char *data = (char *)key;
  size_t key_size = strlen(data);
  int h = (int)key_size;
  for (size_t i = 0; i < key_size; i++) {
    h = 31 * h + *data;
    data++;
  }
  return h;
}

static inline bool default_is_keys_equal(void *key1, void *key2) {
  return strcmp((const char *)key1, (const char *)key2) == 0;
}

hashmap *hashmap_create(size_t capacity, int (*hash)(void *key),
                        bool (*is_keys_equal)(void *key1, void *key2)) {
  hashmap *map = (hashmap *)malloc(sizeof(hashmap));
  if (map == NULL) {
    return NULL;
  }

  // 0.75 load factor
  size_t min_bucket_count = capacity * 4 / 3;
  map->bucket_count = 1;
  while (map->bucket_count < min_bucket_count) {
    // Bucket count must be power of 2
    map->bucket_count <<= 1;
  }

  map->buckets = (entry **)(calloc(map->bucket_count, sizeof(entry *)));
  if (map->buckets == NULL) {
    free(map);
    return NULL;
  }

  map->size = 0;

  map->hash = (hash == NULL ? default_hash_key : hash);
  map->is_keys_equal =
      (is_keys_equal == NULL ? default_is_keys_equal : is_keys_equal);

  pthread_mutex_init(&map->lock, NULL);

  return map;
}

void hashmap_free(hashmap *map) {
  for (size_t i = 0; i < map->bucket_count; i++) {
    entry *e = map->buckets[i];
    while (e != NULL) {
      entry *next = e->next;
      free(e);
      e = next;
    }
  }
  free(map->buckets);
  pthread_mutex_destroy(&map->lock);
  free(map);
}

static inline int hash_key(hashmap *map, void *key) {
  int h = map->hash(key);
  return h;
}

static inline bool is_keys_equal(void *key1, int hash1, void *key2, int hash2,
                                 bool (*equals)(void *, void *)) {
  if (key1 == key2) {
    return 1;
  }
  if (hash1 != hash2) {
    return 0;
  }
  return equals(key1, key2);
}

static inline size_t calculate_index(size_t bucket_count, int hash) {
  return ((size_t)hash) & (bucket_count - 1);
}

static entry *create_entry(void *key, void *value, int hash) {
  entry *e = (entry *)malloc(sizeof(entry));
  if (e == NULL) {
    return NULL;
  }
  e->key = key;
  e->value = value;
  e->hash = hash;
  e->next = NULL;
  return e;
}

static void expand_if_needed(hashmap *map) {
  if (map->size > (map->bucket_count * 3 / 4)) {
    size_t new_bucket_count = map->bucket_count << 1;
    entry **new_buckets = (entry **)calloc(new_bucket_count, sizeof(entry *));
    if (new_buckets == NULL) {
      return;
    }
    // Move over existing entries.
    for (size_t i = 0; i < map->bucket_count; i++) {
      entry *e = map->buckets[i];
      while (e != NULL) {
        entry *next = e->next;
        size_t index = calculate_index(new_bucket_count, e->hash);
        e->next = new_buckets[index];
        new_buckets[index] = e;
        e = next;
      }
    }
    free(map->buckets);
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;
  }
}

void *hashmap_set(hashmap *map, void *key, void *value) {
  int hash = map->hash(key);
  size_t index = calculate_index(map->bucket_count, hash);
  entry **p = &(map->buckets[index]);
  while (true) {
    entry *current = *p;
    // Add a new entry
    if (current == NULL) {
      *p = create_entry(key, value, hash);
      if (*p == NULL) {
        return NULL;
      }
      map->size++;
      expand_if_needed(map);
      return NULL;
    }
    // Replace existing entry
    if (is_keys_equal(current->key, current->hash, key, hash,
                      map->is_keys_equal)) {
      void *old_value = current->value;
      current->value = value;
      return old_value;
    }

    p = &current->next;
  }
}

void *hashmap_get(hashmap *map, void *key) {
  int hash = hash_key(map, key);
  size_t index = calculate_index(map->bucket_count, hash);
  entry *e = map->buckets[index];
  while (e != NULL) {
    if (is_keys_equal(e->key, e->hash, key, hash, map->is_keys_equal)) {
      return e->value;
    }
    e = e->next;
  }
  return NULL;
}

void *hashmap_remove(hashmap *map, void *key) {
  int hash = hash_key(map, key);
  size_t index = calculate_index(map->bucket_count, hash);
  entry **p = &(map->buckets[index]);
  entry *current;
  while ((current = *p) != NULL) {
    if (is_keys_equal(current->key, current->hash, key, hash,
                      map->is_keys_equal)) {
      void *value = current->value;
      *p = current->next;
      free(current);
      map->size--;
      return value;
    }
    p = &current->next;
  }
  return NULL;
}

void hashmap_iterate(hashmap *map, bool (*callback)(void *key, void *value)) {
  for (size_t i = 0; i < map->bucket_count; i++) {
    entry *e = map->buckets[i];
    while (e != NULL) {
      entry *next = e->next;
      if (!callback(e->key, e->value)) {
        return;
      }
      e = next;
    }
  }
}

size_t hashmap_count(hashmap *map) { return map->size; }

void hashmap_lock(hashmap *map) { pthread_mutex_lock(&map->lock); }

void hashmap_unlock(hashmap *map) { pthread_mutex_unlock(&map->lock); }
