#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_COMPARATOR compare

typedef int (*Comparator)(void*, void*);

bool equals(void*, void*);

int compare(void*, void*);

char* to_string(void*);

#endif