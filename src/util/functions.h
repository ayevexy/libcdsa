#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_COMPARATOR compare

typedef int (*Comparator)(void*, void*);

bool equals(void*, void*);

int compare(void* a, void* b);

#endif