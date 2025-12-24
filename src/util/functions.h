#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef void (*Consumer)(void*);

typedef bool (*Predicate)(void*);

typedef void* (*UnaryOperator)(void*);

typedef int (*Comparator)(void*, void*);

bool pointer_equals(void*, void*);

#define DEFAULT_COMPARATOR(T) _Generic((T*) 0,  \
    char*: compare_chars,                       \
    int*: compare_ints,                         \
    long*: compare_longs,                       \
    float*: compare_floats,                     \
    double*: compare_doubles,                   \
    void*: compare_pointers                     \
)

int compare_chars(void*, void*);

int compare_ints(void*, void*);

int compare_longs(void*, void*);

int compare_floats(void*, void*);

int compare_doubles(void*, void*);

int compare_pointers(void*, void*);

#define DEFAULT_TO_STRING(T) _Generic((T*) 0,   \
    char*: char_to_string,                      \
    int*: int_to_string,                        \
    long*: long_to_string,                      \
    float*: float_to_string,                    \
    double*: double_to_string,                  \
    void*: pointer_to_string                    \
)

char* char_to_string(void*);

char* int_to_string(void*);

char* long_to_string(void* e);

char* float_to_string(void* e);

char* double_to_string(void* e);

char* pointer_to_string(void*);

#endif