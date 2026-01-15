# Error Handling

The library provides a built-in error handling mechanism through `util/error.h`. 
By default, when a function fails, it will abruptly terminate the program and print a detailed error message to `stderr`.
To handle errors gracefully, you can use the `attempt` macro. For example:

```c++
// Attempt to execute a function and capture any resulting error
ArrayList* list; Error error = attempt(list = array_list_new(&options));

if (error == ILLEGAL_ARGUMENT_ERROR) {       // Handle illegal argument(s)
    printf("%s\n", error_message());         // e.g., "Error at array_list_new(): illegal argument(s)"
}

if (error == MEMORY_ALLOCATION_ERROR) {      // Handle memory allocation failure
    printf("%s\n", error_message());         // e.g., "Error at array_list_new(): memory allocation failed"
}
```

The attempt macro allows your program to recover from errors instead of crashing, giving you control over how different error types are handled.