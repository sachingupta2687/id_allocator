#include <stdio.h>
#include <string.h>
#include "id_allocator.h"

void test_create_and_query() {
    printf("\n==== TEST 1: Create and Query ====\n");

    int id = create_id("test1");
    if (id == -1) printf("FAIL: create test1\n");

    printf("Querying id %d: ", id);
    query_id(id);
}

void test_duplicate_create() {
    printf("\n==== TEST 2: Duplicate Create ====\n");

    int id1 = create_id("test2");
    int id2 = create_id("test2");

    if (id1 != id2) {
        printf("FAIL: duplicate create returned different IDs (%d vs %d)\n", id1, id2);
    } else {
        printf("PASS: duplicate create returned same id %d\n", id1);
    }
}

void test_delete_and_recreate() {
    printf("\n==== TEST 3: Delete and Recreate ====\n");

    int id1 = create_id("test3");
    printf("Created id=%d\n", id1);

    delete_id(id1);
    printf("Deleted id=%d\n", id1);

    printf("Query deleted id: ");
    query_id(id1);

    printf("Recreating immediately (should NOT reuse same id):\n");
    int id2 = create_id("test3");

    printf("New id = %d\n", id2);
    if (id1 == id2) {
        printf("FAIL: ID reused too early!\n");
    } else {
        printf("PASS: Did NOT reuse recently deleted ID\n");
    }
}

void test_multiple_keys() {
    printf("\n==== TEST 5: Multiple Different Keys ====\n");

    char buf[32];
    int ids[20];
    int i;

    for (i = 0; i < 10; i++) {
        snprintf(buf, sizeof(buf), "key_%d", i);
        ids[i] = create_id(buf);
        printf("Created key %s id %d\n", buf, ids[i]);
    }

    printf("Verifying queries:\n");
    for (i = 0; i < 10; i++) {
        printf("query_id(%d) key: ", ids[i]);
        query_id(ids[i]);
    }
}

void test_invalid_inputs() {
    printf("\n==== TEST 6: Invalid Inputs ====\n");

    printf("Query invalid id -1:\n");
    query_id(-1);

    printf("Delete invalid id 10000:\n");
    delete_id(10000);

    printf("Delete unused id 999\n");
    delete_id(999);

    printf("Create key that is too long:\n");
    char longkey[200];
    memset(longkey, 'A', 199);
    longkey[199] = '\0';
    create_id(longkey);
}

int main() {
    printf("\n===== Running Unit Tests for ID Allocator =====\n\n");

    int range = MAX_ID - MIN_ID + 1;
    id_allocator_init(range);

    test_create_and_query();
    test_duplicate_create();
    test_delete_and_recreate();
    test_multiple_keys();
    test_invalid_inputs();

    printf("\n===== ALL TESTS COMPLETED =====\n\n");
    return 0;
}
