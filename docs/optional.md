# Optional

The library provides the `Optional` type to represent values that may or may not be present, 
especially in cases where `nullptr` is a valid return value. It is defined as follows:

```c++
typedef struct {
    void* const value;
    const bool value_present;
} Optional;
```

An `Optional` can be used like this:

```c++
Optional result = array_list_find(list, predicate);

if (result.value_present) {
    printf("value: %d\n", *(int*) result.value); // 10 
}
```

NOTE: Empty optionals returned by the library functions points to an arbitrary invalid memory address,
trying to deference it will usually crash the program, i.e. it's undefined behavior.