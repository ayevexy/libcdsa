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

extern const char* error_strings[];

const char* error_message(void);

const char* plain_error_message(void);

#define attempt(expression) (isolate_error(), (expression), capture_error())

void isolate_error(void);

Error capture_error(void);

#define raise_error(error, message, ...)                                            \
    raise_plain_error(error,                                                        \
        message[0] == '\0' ? "Error at %s(): %s" : "Error at %s(): %s - "message,   \
        __func__,                                                                   \
        error_strings[error],                                                       \
        ##__VA_ARGS__                                                               \
    ); return

void raise_plain_error(Error error, const char* error_message_format, ...);

#endif
