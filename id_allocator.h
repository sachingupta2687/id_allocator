#include <stdio.h>
#include <stdint.h>

#define MIN_ID 1
#define MAX_ID 2000

int create_id(const char *key);

void delete_id(int id);

void query_id(int id);

void id_allocator_init(int range);
