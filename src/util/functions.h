#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef void (*Consumer)(void*);

typedef bool (*Predicate)(void*);

typedef void* (*UnaryOperator)(void*);

typedef int (*Comparator)(void*, void*);

bool default_equals(void*, void*);

#define DEFAULT_COMPARATOR(T) _Generic((T*) 0,  \
    char*: char_compare,                        \
    int*: int_compare,                          \
    long*: long_compare,                        \
    float*: float_compare,                      \
    double*: double_compare,                    \
    void*: default_compare                      \
)

int char_compare(void*, void*);

int int_compare(void*, void*);

int long_compare(void*, void*);

int float_compare(void*, void*);

int double_compare(void*, void*);

int default_compare(void*, void*);

#define DEFAULT_TO_STRING(T) _Generic((T*) 0,   \
    char*: char_to_string,                      \
    int*: int_to_string,                        \
    long*: long_to_string,                      \
    float*: float_to_string,                    \
    double*: double_to_string,                  \
    void*: default_to_string                    \
)

char* char_to_string(void*);

char* int_to_string(void*);

char* long_to_string(void* e);

char* float_to_string(void* e);

char* double_to_string(void* e);

char* default_to_string(void*);

#endif