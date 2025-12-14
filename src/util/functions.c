#include "functions.h"

bool equals(void* a, void* b) {
    return a == b;
}

int compare(void* a, void* b) {
    return (a > b) - (a < b);
}
