#include "map/hash_map.h"
#include "util/errors.h"
#include "../test_functions.h"

#include "unity.h"

#define CHAR_INT_HASH_MAP_OPTIONS DEFAULT_HASH_MAP_OPTIONS(     \
    .key_destruct = free,                                       \
    .key_equals = char_pointer_value_equals,                    \
    .key_to_string = char_pointer_value_to_string,              \
    .value_destruct = free,                                     \
    .value_equals = int_pointer_value_equals,                   \
    .value_to_string = int_pointer_value_to_string              \
)

static HashMap* hash_map;

void setUp() {
    hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
}

void tearDown() {
    hash_map_obliterate(&hash_map);
}

void test_create_hash_map() {
    TEST_ASSERT_NOT_NULL(hash_map);
}

void test_do_not_create_hash_map_with_invalid_options() {
    // when
    HashMap* new_hash_map; Error error = attempt(new_hash_map = hash_map_new(&(HashMapOptions) {}));
    // then
    TEST_ASSERT_NULL(new_hash_map);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_hash_map_set_it_to_null() {
    // given
    HashMap* new_hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
    // when
    hash_map_destroy(&new_hash_map);
    // then
    TEST_ASSERT_NULL(new_hash_map);
}

void test_destroy_null_hash_map_fails() {
    // given
    HashMap* new_hash_map = nullptr;
    // when
    Error error = attempt(hash_map_destroy(&new_hash_map));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_map);
    RUN_TEST(test_do_not_create_hash_map_with_invalid_options);
    RUN_TEST(test_destroy_hash_map_set_it_to_null);
    RUN_TEST(test_destroy_null_hash_map_fails);
    return UNITY_END();
}