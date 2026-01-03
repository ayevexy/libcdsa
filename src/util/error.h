#ifndef ERROR_H
#define ERROR_H

typedef enum {
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    NO_SUCH_ELEMENT_ERROR,
    INVALID_ARGUMENTS_ERROR,
    MEMORY_ALLOCATION_ERROR
} Error;

const char* error_message(void);

#define attempt(expression) (set_error(NO_ERROR, "\0"), (expression), get_error())

Error get_error(void);

void set_error(Error error, const char* error_message_format, ...);

#endif
