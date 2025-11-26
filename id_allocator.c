#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include "id_allocator.h"
#include "hashmap.h"
#include "bitmap.h"
#include "utils.h"

/* Assumption: Max 127 byte key size */
#define MAX_KEY_SIZE 127


int allocate_id(const char *key) {
  int id = bitmap_allocate_id();
  
  DEBUG_LOG("%s %d bitarray %u\n", __func__, __LINE__, bitmap.bit_array[0]);
  if (id == -1) {
    printf("Id allocation failed\n");
    return -1;
  }
  
  hash_insert_key(key, id);
  return id;
}

/* Id map array */
char id_key_array[MAX_ID - MIN_ID + 1][MAX_KEY_SIZE + 1];
pthread_mutex_t idmap_lock;

int get_id(const char *key) {
  int id = -1; 
  int hash_index;
  
  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  hash_index = hash_get_index(key);
  id = hash_find_entry(hash_index, key);
  
  return id;
}

void id_allocator_init(int range) {
  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  memset(id_key_array, 0, sizeof(id_key_array));
  hash_init(range);
  bitmap_init(range);
  pthread_mutex_init(&idmap_lock, NULL);
}

int create_id(const char *key) {
  int id;
  
  DEBUG_LOG("%s %d key %s len %d\n", __func__, __LINE__, key, strlen(key)); 
  //validate key
  if (strlen(key) > MAX_KEY_SIZE) {
    printf("Invalid key size, max allowed key size %d\n", MAX_KEY_SIZE);
    return -1;
  }
  
  //if key already exists, get its id
  id = get_id(key);
  if (id != -1) {
    printf("Entry already exists with id %d\n", id);
    return id;
  } 

  id = allocate_id(key);
  if ((id < MIN_ID) || (id > MAX_ID)) {
    printf("Invalide id %d allocated\n", id);
    return -1;
  }
  
  pthread_mutex_lock(&idmap_lock);
  strcpy(id_key_array[id], key);
  pthread_mutex_unlock(&idmap_lock);
  
  printf("created id %d for key %s\n", id, key);
  return id;
}

void delete_id(int id) {
  char key[MAX_KEY_SIZE + 1];

  if ((id < MIN_ID) || (id > MAX_ID)) {
    printf("Invalid Id\n");
    return;
  }
  
  pthread_mutex_lock(&idmap_lock);
  strcpy(key, id_key_array[id]);
  /*remove from id_key_map table
    shouldn't st id_key_array[id] = '\0'
	else it will start pointiing to a different memory
	instead of filling null in allocated memory */
  memset(id_key_array[id], 0, MAX_KEY_SIZE + 1);
  pthread_mutex_unlock(&idmap_lock);
  
  //first remove id from hash function
  DEBUG_LOG("%s %d key=%s\n", __func__, __LINE__, key); 
  
  hash_remove_key(key);
  //free id from bitmap
  bitmap_free_id(id);
  return;
}

void query_id(int id) {
  DEBUG_LOG("%s %d id %d\n", __func__, __LINE__, id); 
  if ((id < MIN_ID) || (id > MAX_ID)) {
    printf("Invalid Id\n");
    return;
  }

  DEBUG_LOG("%s %d\n", __func__, __LINE__);
  
  pthread_mutex_lock(&idmap_lock);
  if (id_key_array[id][0] == '\0') {
    pthread_mutex_unlock(&idmap_lock);  
    printf("Id %d doens't exists\n", id);
    return;
  }
  
  printf("%s\n", id_key_array[id]);
  pthread_mutex_unlock(&idmap_lock);
  return;
}

void print_all_ids() {
	int id;
    
    pthread_mutex_lock(&idmap_lock);
	for (id=MIN_ID; id<=MAX_ID; id++) {
		if (id_key_array[id][0] != '\0') {
			printf("%d %s\n", id, id_key_array[id]);
		}
	}
	pthread_mutex_unlock(&idmap_lock);
}
