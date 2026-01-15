#ifndef ERROR_H
#define ERROR_H

typedef enum {
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    NO_SUCH_ELEMENT_ERROR,
    ILLEGAL_ARGUMENT_ERROR,
    MEMORY_ALLOCATION_ERROR
} Error;

const char* error_message(void);

#define attempt(expression) (isolate_error(), (expression), capture_error())

void isolate_error(void);

Error capture_error(void);

void raise_error(Error error, const char* error_message_format, ...);

#endif
