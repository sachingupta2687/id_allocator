#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include "hashmap.h"
#include "utils.h"

hash hash_table;
pthread_mutex_t hash_lock;

//we should keep hash table size equal to 2*(max_id - min_id), to keep load_factor=0.5. 
//It helps in keeping the collisions minimum.
void hash_init(int range) {
  hash_table.size = 0;
  hash_table.max_size = range*HASH_ARRAY_LOAD_FACTOR;
  hash_table.hash_array = (hash_node **)calloc(hash_table.max_size, sizeof(hash_node *));
  pthread_mutex_init(&hash_lock, NULL);
  
  return;
}

static hash_node* hash_entry_insert(hash_node *head, const char *key, int id) {
  hash_node *temp = NULL;
  int key_size = 0;

  key_size = strlen(key);
  if (key_size > MAX_HASH_KEY_SIZE) {
    printf("%s: Too big key %d, max key size %d\n", 
            __func__, key_size, MAX_HASH_KEY_SIZE);
    return head;
  }
  
  temp = (hash_node *)malloc(sizeof(hash_node));
  if (temp == NULL) {
    printf("memory allocation failed %s %d\n", __func__, __LINE__);
    return head;
  }

  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  
  //copy data to the node
  strcpy(temp->key, key); //strcpy will also copy the null char.
  temp->id = id;
  temp->len = key_size;
  
  //insert at the front of link-list
  temp->next = head;
  return temp;
}

static hash_node* hash_remove_entry(hash_node *head, char *key) {
  hash_node *cur = head;
  hash_node *temp = NULL;
  
  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  if (cur == NULL) {
    //printf("Entry not found\n");
    return NULL;
  }
    
  if (strcmp(cur->key, key) == 0) {
    head = cur->next;
    free(cur);
    return head;
  }
  
  while (cur->next && (strcmp(cur->next->key, key) != 0)) {
    cur = cur->next;
  }
  
  if (cur->next == NULL) {
    printf("%s: Entry not found\n", __func__);
    return head;
  }
  
  temp = cur->next;
  cur->next = temp->next;
  free(temp);
  return head;
}


/* Using a simple hash functions.
 * There are better hash functions available */
static uint64_t hash_func(const char *key) {
    const char *ch = key;
    uint64_t hash = 0;
    int i = 1;
  
    DEBUG_LOG("%s %d\n", __func__, __LINE__); 
    while (*ch != '\0') {
        hash += ((*ch)^2)*i;
        ch ++;
        i++;
    }
  
    return hash;
}

int hash_get_index(const char *key) {
  uint64_t hash = 0;
  int hash_index = 0; 
  
  hash = hash_func(key);
  hash_index = hash % hash_table.max_size;
  DEBUG_LOG("%s %d hash=%lu hash_index=%d\n", __func__, __LINE__, hash, hash_index); 
  return hash_index;
}

void hash_insert_key(const char *key, int id) {
  hash_node *head = NULL;
  int hash_index = 0;

  hash_index = hash_get_index(key);
  DEBUG_LOG("%s %d\n", __func__, __LINE__);
  
  pthread_mutex_lock(&hash_lock);
  head = hash_table.hash_array[hash_index];
  head = hash_entry_insert(head, key, id);
  hash_table.hash_array[hash_index] = head;
  pthread_mutex_unlock(&hash_lock);
  return;
}


void hash_remove_key(char *key) {
  int hash_index = 0; 
  hash_node *head = NULL;
  
  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  hash_index = hash_get_index(key);
  
  pthread_mutex_lock(&hash_lock);
  head = hash_table.hash_array[hash_index];
  head = hash_remove_entry(head, key);
  hash_table.hash_array[hash_index] = head;
  pthread_mutex_unlock(&hash_lock);
  return;
}

int hash_find_entry(int hash_index, const char *key) {
  hash_node *cur = NULL;
  int id = -1;
  
  DEBUG_LOG("%s %d\n", __func__, __LINE__);
  
  pthread_mutex_lock(&hash_lock);
  cur = hash_table.hash_array[hash_index];
  while (cur != NULL) {
    if (strcmp(cur->key, key) == 0) {
      id = cur->id;
      break;
    }
    cur = cur->next;
  }
  pthread_mutex_unlock(&hash_lock);
  
  return id;
}

