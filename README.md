# id_allocator
Id management library with functionalities create_id, delete_id and query_id.

Files Included: 
bitmap.h, hashmap.h, id_allocator.h, utils.h, bitmap.c, hashmap.c, id_allocator.c, test_driver.c

Test Driver: 
It includes 6 test cases covering different scenarios. Details of each executed test case can be seen in logs.

Steps to run:
1. git clone https://github.com/sachingupta2687/id_allocator.git
2. cd id_allocator
3. Run below command
   gcc test_driver.c id_allocator.c hashmap.c bitmap.c
4. Run executable file
   ./a.out


Parameters considered:
MIN_ID - 1
MAX_ID - 2000
MAX_KEY_SIZE - 127
FREE_ID_COOL_OFF_TIME - 3 second


Key Design Considerations:
1. Hash table to map key to id. Collision resolution is handled using hashing with chaining.
2. Direct array for id to key mapping.
3. Bitmap with timestamps for efficient ID allocation and delayed reuse.
4. Separate Mutex/Locks are used for hash table, bitmap and id map array.


Design Tradeoffs:
Hash table provides very fast lookup of the O(1). But as the number of entries increase in the table, number of collisions will also increase. 
A load factor of 0.5 is considered to reduce the collisions which will consume more memory.

Array for id key will also provide O(1) lookup. Advance allocation of memory is required. If number of ids used are less, there will be wastage of memory.

Bitmap array is highly memory efficient. It is also very fast due to the bit operations. But in worst case, it may take O(n) to allocate id.

Thread-safety is achieved using fine-grained locks across hash table, bitmap allocator, and id map array, maximizing concurrency while avoiding race conditions.


Summary of each function:
1. create_id(key)
Check hash table: if key exists, return existing ID.
Allocate new ID using bitmap allocator.
Store key in id_key map array
Inserts key->id mapping in hash table

2. delete_id(id)
Lookup key in id_key_array
Remove entry from hash table
Clear id_key_array slot
Free ID in bitmap (sets timestamp)

3. query_id(id)
Direct lookup in id_key_array.

