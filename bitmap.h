#include <stdio.h>
#include <stdint.h>

#ifndef BITMAP_H
#define BITMAP_H


/* Bitmap id allocator */
typedef struct bitmap_t {
  int size;
  int max_size;
  int next_free;
  char *bit_array;
  uint64_t *ts_array; //timestamp array
} bitmap_t;

void bitmap_init(int range);

int bitmap_allocate_id(void);

void bitmap_free_id(int id);


#endif //BITMAP_H
