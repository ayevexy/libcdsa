#ifndef ITERATOR_TEST_H
#define ITERATOR_TEST_H

#include "../test_utilities.h"

#include "list/array_list.h"
#include "list/linked_list.h"
#include "map/hash_map.h"
#include "set/hash_set.h"

#define ARRAY_LIST 0
#define LINKED_LIST 1
#define HASH_MAP 2
#define HASH_SET 3

typedef MapEntry Element;

#if DATA_STRUCTURE == ARRAY_LIST

    typedef ArrayList DataStructure;

    #define DATA_STRUCTURE_NEW                          array_list_new
    #define DATA_STRUCTURE_DESTROY                      array_list_destroy
    #define DATA_STRUCTURE_ITERATOR                     array_list_iterator
    #define DATA_STRUCTURE_CLEAR                        array_list_clear
    #define DATA_STRUCTURE_DEFAULT_OPTS                 DEFAULT_ARRAY_LIST_OPTIONS

    static Element expected_elements[] = {
        { &(int){1} }, { &(int){2} }, { &(int){3} }, { &(int){4} }, { &(int){5} }
    };

#elif DATA_STRUCTURE == LINKED_LIST

    typedef LinkedList DataStructure;

    #define DATA_STRUCTURE_NEW                          linked_list_new
    #define DATA_STRUCTURE_DESTROY                      linked_list_destroy
    #define DATA_STRUCTURE_ITERATOR                     linked_list_iterator
    #define DATA_STRUCTURE_CLEAR                        linked_list_clear
    #define DATA_STRUCTURE_DEFAULT_OPTS                 DEFAULT_LINKED_LIST_OPTIONS

    static Element expected_elements[] = {
        { &(int){1} }, { &(int){2} }, { &(int){3} }, { &(int){4} }, { &(int){5} }
    };

#elif DATA_STRUCTURE == HASH_MAP

    typedef HashMap DataStructure;

    #define DATA_STRUCTURE_NEW                      hash_map_new
    #define DATA_STRUCTURE_DESTROY                  hash_map_destroy
    #define DATA_STRUCTURE_ITERATOR                 hash_map_iterator
    #define DATA_STRUCTURE_CLEAR                    hash_map_clear
    #define DATA_STRUCTURE_DEFAULT_OPTS             DEFAULT_HASH_MAP_OPTIONS

    static Element expected_elements[] = {
        { &(int){4} }, { &(int){5} }, { &(int){1} }, { &(int){2} }, { &(int){3} }
    };

#elif DATA_STRUCTURE == HASH_SET

    typedef HashSet DataStructure;

    #define DATA_STRUCTURE_NEW                      hash_set_new
    #define DATA_STRUCTURE_DESTROY                  hash_set_destroy
    #define DATA_STRUCTURE_ITERATOR                 hash_set_iterator
    #define DATA_STRUCTURE_CLEAR                    hash_set_clear
    #define DATA_STRUCTURE_DEFAULT_OPTS             DEFAULT_HASH_SET_OPTIONS

    static Element expected_elements[] = {
        { &(int){5} }, { &(int){4} }, { &(int){3} }, { &(int){2} }, { &(int){1} }
    };

#endif

static Element elements[] = {
    { &(int){1} }, { &(int){2} }, { &(int){3} }, { &(int){4} }, { &(int){5} }
};

#define POPULATE_DATA_STRUCTURE(data_structure)                                                     \
    for (int i = 0; i < SIZE(elements); i++) {                                                      \
        _Generic((data_structure),                                                                  \
            ArrayList*: array_list_add_last((ArrayList*) data_structure, &elements[i]),             \
            LinkedList*: linked_list_add_last((LinkedList*) data_structure, &elements[i]),          \
            HashMap*: hash_map_put((HashMap*) data_structure, elements[i].key, elements[i].value),  \
            HashSet*: hash_set_add((HashSet*) data_structure, &elements[i])                         \
        );                                                                                          \
    }

#define FOR_EACH_ELEMENT_OF_DATA_STRUCTURE(element, data_structure)                 \
    TEST_ASSERT_GREATER_THAN(0, SIZE(expected_elements));                           \
    for (int i = 0; i < SIZE(expected_elements); i++)                               \
        for (MapEntry* element = &expected_elements[i]; element; element = nullptr)

#define FOR_EACH_ELEMENT_OF_DATA_STRUCTURE_REVERSED(element, data_structure)        \
    TEST_ASSERT_GREATER_THAN(0, SIZE(expected_elements));                           \
    for (int i = SIZE(expected_elements) - 1; i > 0; i--)                           \
        for (MapEntry* element = &expected_elements[i]; element; element = nullptr)

#define TEST_ASSERT_EQUAL_ELEMENT(expected, actual)                     \
    do {                                                                \
        MapEntry* _entry = (actual);                                    \
        TEST_ASSERT_EQUAL(*(int*) expected->key, *(int*) _entry->key);  \
        TEST_ASSERT_NULL(_entry->value);                                \
    } while (false)

#endif