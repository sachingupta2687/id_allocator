#include <stdio.h>
#include <stdint.h>

#ifndef HASHMAP_H
#define HASHMAP_H

#define MAX_HASH_KEY_SIZE 127
#define HASH_ARRAY_LOAD_FACTOR 2

/* Hash Library */
typedef struct hash_node_t {
  char key[MAX_HASH_KEY_SIZE + 1]; // +1 for NULL char
  int len; //key length
  int id;
  struct hash_node_t *next;
} hash_node;

typedef struct hash_t {
  int size; //current size
  int max_size;
  hash_node **hash_array;
} hash;

void hash_init(int range);

int hash_get_index(const char *key);

void hash_insert_key(const char *key, int id);

void hash_remove_key(char *key);

int hash_find_entry(int hash_index, const char *key);

#endif //HASHMAP_H
