# Error Handling

The library has a built-in simple error handling mechanism provided by `util/error.h` inspired on `errno.h` from the standard library.
It is implemented by two `_Thread_local` global variables: `global_error` and `global_error_message`, when a function fail
to do something, it will return a default value (`O`, `false` or `nullptr`), but since a default value is not sufficient
to indicate an error (default values could indicate no errors at all) it will set those two variables with more detailed information
about the exact failure. The following example show how it works within the `ArrayList` module:

```c++
global_error = NO_ERROR;                            // ensure global error is in valid state

ArrayList* list = array_list_new(&options);         // array list creation failed, list is null

if (global_error == INVALID_ARGUMENTS_ERROR) {      // handle INVALID_ARGUMENTS_ERROR
    printf("%s", global_error_message);             // Error at array_list_new(): invalid argument(s)
    global_error = NO_ERROR;                        // reset global error state
}

if (global_error == MEMORY_ALLOCATION_ERROR) {      // handle MEMORY_ALLOCATION_ERROR
    printf("%s", global_error_message);             // Error at array_list_new(): memory allocation failure!
    global_error = NO_ERROR;                        // reset global error state
}
```

The above code snippet is error-prone since you have to ensure `global_error` is in a valid state everytime before calling a function.
Also, checking a global variable outside the context of a function looks like it's not part of the API itself.
Luckily the library provides the `attempt` macro to hide this complexity:

```c++
// attempt to execute a function expression and capture its error
ArrayList* list; Error error = attempt(array_list = array_list_new(&options));

if (error == INVALID_ARGUMENTS_ERROR) {     // handle INVALID_ARGUMENTS_ERROR
    printf("%s", error_message());          // Exception at array_list_new(): invalid argument(s)
}
if (error == MEMORY_ALLOCATION_ERROR) {     // handle MEMORY_ALLOCATION_ERROR
    printf("%s", error_message());          // Fatal Error at array_list_new(%p): memory allocation failure!
}
```

The snippet above expands to the following code:

```c++
ArrayList* list;

Error error = (
    set_error(NO_ERROR, "\0"),              // ensure global error is in a valid state
    (list = array_list_new(options)),       // execute the function expression which may set an error
    get_error()                             // return the captured error and reset global error state
);

if (error == INVALID_ARGUMENTS_ERROR) {     // handle INVALID_ARGUMENTS_ERROR
    printf("%s", global_error_message);     // Exception at array_list_new(): invalid argument(s)
}

if (error == MEMORY_ALLOCATION_ERROR) {     // handle MEMORY_ALLOCATION_ERROR
    printf("%s", global_error_message);     // Fatal Error at array_list_new(%p): memory allocation failure!
}
```