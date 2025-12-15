#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_COMPARATOR default_compare

typedef void (*Consumer)(void*);

typedef int (*Comparator)(void*, void*);

bool default_equals(void*, void*);

int default_compare(void*, void*);

char* default_to_string(void*);

#endif