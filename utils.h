#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_LOG(fmt, ...) printf("[DEBUG]" fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...)
#endif
