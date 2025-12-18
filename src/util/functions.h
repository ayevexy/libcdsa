#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_COMPARATOR(T) _Generic((T*) 0,  \
    char*: char_compare,                        \
    int*: int_compare,                          \
    long*: long_compare,                        \
    float*: float_compare,                      \
    double*: double_compare,                    \
    void*: default_compare                      \
)

typedef void (*Consumer)(void*);

typedef int (*Comparator)(void*, void*);

bool default_equals(void*, void*);

int default_compare(void*, void*);

char* default_to_string(void*);

int char_compare(void*, void*);

int int_compare(void*, void*);

int long_compare(void*, void*);

int float_compare(void*, void*);

int double_compare(void*, void*);

#endif