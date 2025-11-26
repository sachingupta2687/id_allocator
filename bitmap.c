#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include "utils.h"
#include "bitmap.h"


#define FREE_ID_COOL_OFF_TIME 3000000 //3 sec defined in microseconds

bitmap_t bitmap; //declare a global bitmap for id allocation
pthread_mutex_t bitmap_lock;

uint64_t get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t)tv.tv_sec * 1000000ULL) + tv.tv_usec;
}

static int bitmap_set_index(int index) {
  int arr_idx = 0;
  int bit_offset = 0;
  uint8_t mask = 0;
  uint64_t now = 0;
  
  //DEBUG_LOG("%s %d index %d\n", __func__, __LINE__, index); 
  DEBUG_LOG("%s %d index %d bit_array[0] %u\n", __func__, __LINE__, index, bitmap.bit_array[0]); 
  arr_idx = index/8;
  bit_offset = index%8;
  mask = 1 << bit_offset;
  
  if ((bitmap.bit_array[arr_idx] & mask) != 0) {
    return 0;
  }
  
  /* id is free check timestamp */
  now = get_time_us();
  if ((bitmap.ts_array[index] == 0) || ((now - bitmap.ts_array[index]) > FREE_ID_COOL_OFF_TIME)) {
    bitmap.bit_array[arr_idx] |= mask; //set the bit
    return 1;
  }
  
  return 0;
}

void bitmap_init(int range) {
  memset(&bitmap, 0, sizeof(bitmap_t));
  /*keep size and next_free as 0, initialize others */
  bitmap.max_size = range+1; //we are not using 0th bit
  bitmap.size = 1; //taking account of zero.
  
  int array_size = (bitmap.max_size + 7)/8;
  bitmap.bit_array = (char *)calloc(array_size, sizeof(char));
  
  if (bitmap.bit_array == NULL) {
    printf("%s: Memory allocation failed for bitmap array\n", __func__);
  }
	  
  bitmap.ts_array = (uint64_t *)calloc(bitmap.max_size, sizeof(uint64_t));
  if (bitmap.ts_array == NULL) {
    printf("%s: Memory allocation failed for ts_array\n", __func__);
  }
  
  pthread_mutex_init(&bitmap_lock, NULL);
  
  //set id 0 and never free it, so that it doesn't get allocated.
  bitmap_set_index(0);
  
  return;
}

      
int bitmap_allocate_id() {
  int i=0, index=0;
  
  DEBUG_LOG("%s %d\n", __func__, __LINE__); 
  
  pthread_mutex_lock(&bitmap_lock);
  if (bitmap.size == bitmap.max_size) {
    printf("No IDs available\n");
    pthread_mutex_unlock(&bitmap_lock);
    return -1;
  }
  
  for (i=0; i < bitmap.max_size; i++) {
    index = (bitmap.next_free + i)%bitmap.max_size; //start searching from next_free
    if (bitmap_set_index(index) == 1) {
      bitmap.size++;
      //next_free shall always go in round and not jump to most recently freed
  	  bitmap.next_free = (index+1)%bitmap.max_size;
  	  pthread_mutex_unlock(&bitmap_lock);
      return index;
    }
  }
  pthread_mutex_unlock(&bitmap_lock);
  
  printf("No Ids Available\n");
  return -1;
}

void bitmap_free_id(int id) {
  uint8_t mask = 0;
  uint64_t now = 0;
  int index = 0;
  int bit_offset = 0;
  
  DEBUG_LOG("%s %d bitarray %u\n", __func__, __LINE__, bitmap.bit_array[0]); 
  index = id/8;
  bit_offset = id%8;

  mask = 1 << bit_offset;
  
  pthread_mutex_lock(&bitmap_lock);
  if ((bitmap.bit_array[index] & mask) == 0) {
    printf("Id is already free. Nothing to be done\n");
  }
  
  now = get_time_us();
  mask = ~mask;
  bitmap.bit_array[index] &= mask;
  bitmap.size--;
  bitmap.ts_array[id] = now;
  pthread_mutex_unlock(&bitmap_lock);
  
  return;
}
