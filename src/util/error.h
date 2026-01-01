#ifndef ERROR_H
#define ERROR_H

#include <string.h>

typedef enum {
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    INVALID_ARGUMENTS_ERROR,
    MEMORY_ALLOCATION_ERROR
} Error;

extern _Thread_local Error global_error;

extern _Thread_local char global_error_message[];

#define ERROR_MESSAGE global_error_message

#define ERROR_MESSAGE_VALUE error_message_value()

static inline char* error_message_value() {
    static _Thread_local char error_message[256];
    strcpy(error_message, global_error_message);
    return error_message;
}

#define attempt(expression) (global_error = NO_ERROR, (expression), attempt_get_error())

static inline Error attempt_get_error(void){
    const Error error = global_error;
    global_error = NO_ERROR;
    return error;
}

void set_error(Error error, const char* format, ...);

#endif
