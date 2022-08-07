#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../hashmap.c"

static void test_hashmap_create() {
  hashmap *map = hashmap_create(12, NULL, NULL);
  assert(map != NULL);
  assert(map->buckets != NULL);
  assert(map->bucket_count == 16);
  assert(map->size == 0);
  assert(map->hash != NULL);
  assert(map->is_keys_equal != NULL);
}

static void test_hashmap_destroy() {
  hashmap *map = hashmap_create(12, NULL, NULL);
  hashmap_destroy(map);
  assert(map != NULL);
  assert(map->buckets == NULL);
}

static void test_hashmap_get_set() {
  hashmap *map = hashmap_create(12, NULL, NULL);
  hashmap_set(map, "k1", "v1");
  hashmap_set(map, "k2", (void *)2);

  assert(strcmp(hashmap_get(map, "k1"), "v1") == 0);
  assert(hashmap_get(map, "k2") == (void *)2);
  assert(hashmap_get(map, "k3") == NULL);
}

static void test_hashmap_get_set2() {
  hashmap *map = hashmap_create(12, NULL, NULL);
  hashmap_set(map, "k1", "v1");
  char *pre = hashmap_set(map, "k1", "v10");
  assert(strcmp(hashmap_get(map, "k1"), "v10") == 0);
  assert(strcmp(pre, "v1") == 0);
}

static void test_hashmap_remove() {
  hashmap *map = hashmap_create(12, NULL, NULL);

  hashmap_set(map, "k1", "v1");
  assert(strcmp(hashmap_get(map, "k1"), "v1") == 0);

  char *removed = hashmap_remove(map, "k1");
  assert(hashmap_get(map, "k1") == NULL);
  assert(strcmp(removed, "v1") == 0);
}

static void test_hashmap_auto_expand() {
  hashmap *map = hashmap_create(3, NULL, NULL);
  assert(map->bucket_count == 4);
  hashmap_set(map, "k1", "v1");
  hashmap_set(map, "k2", "v2");
  hashmap_set(map, "k3", "v3");
  assert(map->bucket_count == 4);
  hashmap_set(map, "k4", "v4");
  assert(map->bucket_count == 8);
}

static bool iterator(void *key, void *value, void *context) {
  if (strcmp(key, "k2") == 0) {
    *(bool *)context = true;
    return false;
  }

  return true;
}

static void test_hashmap_iterate() {
  hashmap *map = hashmap_create(12, NULL, NULL);
  hashmap_set(map, "k1", "v1");
  hashmap_set(map, "k2", "v2");
  hashmap_set(map, "k3", "v3");

  bool found = false;
  hashmap_iterate(map, iterator, &found);
  assert(found == true);
}

int main() {
  test_hashmap_create();
  test_hashmap_destroy();
  test_hashmap_get_set();
  test_hashmap_get_set2();
  test_hashmap_remove();
  test_hashmap_auto_expand();
  test_hashmap_iterate();

  printf("All Done!\n");
  return 0;
}