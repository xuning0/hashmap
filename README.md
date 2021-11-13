# hashmap

[Hashmap](https://en.wikipedia.org/wiki/Hash_table) implementation in C using linked list for collision resolution.

## Usage

### Create

To create a hashmap call the `hashmap_create` function:

```c
hashmap *map = hashmap_create(12, NULL, NULL);
```

If you use a string type key, just pass `NULL` to use the default hash algorithm and compare function. Otherwise, you can also use your own algorithm and function.

### Set

To set an entry in the hashmap use the `hashmap_set` function:

```c
hashmap_set(map, "k1", "v1");
char *pre = hashmap_set(map, "k1", "v2"); // return "v1"
```

### Get

To get an entry from a hashmap use the `hashmap_get` function:

```c
void *value = hashmap_get(map, "k1");
```

### Remove

To remove an entry from a hashmap use the `hashmap_remove` function:

```c
char *removed = hashmap_remove(map, "k1");
```

### Iterate

You can iterate over all the entries stored in the hashmap with the `hashmap_iterate` function:

```c
static bool iterator(void *key, void *value) {
  printf("k: %s, v: %s\n", key, value);
  if (strcmp(key, "k3") == 0) {
    // Stop here
    return false;
  }
  return true;
}

hashmap_iterate(map, iterator);
```

You can early exit from the iteration of the hashmap by returning non-zero from your callback function.

### Get Number of Entries

To get the number of entries stored in the hashmap use the `hashmap_count` function:

```c
size_t count = hashmap_count(map);
```

### Free

To Free a hashmap when you are finished with it use the `hashmap_free` function:

```c
hashmap_free(map);
map = NULL;
```

This does not free the keys or values themselves.

### Thread Safety

For thread safety when getting and setting, use the `hashmap_lock` and `hashmap_unlock` functions:

```c
hashmap_lock(map);
hashmap_set(map, "k1", "v1");
hashmap_unlock(map);
```
