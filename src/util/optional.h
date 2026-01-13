#ifndef OPTIONAL_H
#define OPTIONAL_H

typedef struct {
    void* const value;
    const bool value_present;
} Optional;

#define optional_empty() (Optional) { .value = (void*) 0xDEADBEEF, .value_present = false }

#define optional_of(_value) (Optional) { .value = _value, .value_present = true }

#endif