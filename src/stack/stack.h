#ifndef STACK_H
#define STACK_H

#include "deque/deque.h"

/**
 * A stack is a generic LIFO (Last-In-First-Out) data structure built on top of a deque.
 * It allows insertion of elements at the top (push) and removal from the top (pop),
 * while maintaining the order of elements. Internally, a stack is simply a type alias for a deque.
 */
typedef Deque Stack;

/**
 * Stack configuration structure. Alias of DequeOptions.
 *
 * Use StackOptions to configure:
 * - initial capacity
 * - destruct function
 * - equals function
 * - to_string function
 * - memory allocation functions
 */
typedef DequeOptions StackOptions;

/**
 * @brief Utility macro providing default StackOptions.
 *
 * Alias of DEFAULT_DEQUE_OPTIONS. Optional overrides can be provided.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_STACK_OPTIONS DEFAULT_DEQUE_OPTIONS

/**
 * @brief Creates a new empty stack using the specified options.
 *
 * @param options pointer to a StackOptions structure
 *
 * @return pointer to a newly created stack
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Stack* stack_new(const StackOptions* options) {
    return deque_new(options);
}

/**
 * @brief Creates a new stack containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created stack
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
static inline Stack* stack_from(Collection collection, const StackOptions* options) {
    return deque_from(collection, options);
}

/**
 * @brief Destroys a stack and optionally its elements.
 *
 * @param stack_pointer pointer to a stack pointer
 *
 * @exception NULL_POINTER_ERROR if stack_pointer or *stack_pointer is null
 *
 * @post *stack_pointer == nullptr
 */
static inline void stack_destroy(Stack** stack_pointer) {
    deque_destroy(stack_pointer);
}

/**
 * @brief Sets the element destruct function.
 *
 * @param stack pointer to a stack
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if stack or destruct is null
 */
static inline void stack_set_destructor(Stack* stack, void (*destruct)(void*)) {
    deque_set_destructor(stack, destruct);
}

/**
 * @brief Pushes an element onto the top of the stack.
 *
 * @param stack pointer to a stack
 * @param element element to push
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
static inline void stack_push(Stack* stack, const void* element) {
    deque_add_last(stack, element);
}

/**
 * @brief Pushes all elements of a collection onto the top of the stack.
 *
 * @param stack pointer to a stack
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
static inline void stack_push_all(Stack* stack, Collection collection) {
    deque_add_all_last(stack, collection);
}

/**
 * @brief Retrieves the element at the top of the stack without removing it.
 *
 * @param stack pointer to a stack
 *
 * @return pointer to the top element
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception NO_SUCH_ELEMENT_ERROR if stack is empty
 */
static inline void* stack_peek(const Stack* stack) {
    return deque_get_last(stack);
}

/**
 * @brief Removes and returns the element at the top of the stack.
 *
 * @param stack pointer to a stack
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception NO_SUCH_ELEMENT_ERROR if stack is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
static inline void* stack_pop(Stack* stack) {
    return deque_remove_last(stack);
}

/**
 * @brief Returns the current number of elements in the stack.
 *
 * @param stack pointer to a stack
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline int stack_size(const Stack* stack) {
    return deque_size(stack);
}

/**
 * @brief Returns the current capacity of the stack.
 *
 * @param stack pointer to a stack
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline int stack_capacity(const Stack* stack) {
    return deque_capacity(stack);
}

/**
 * @brief Checks whether the stack is empty.
 *
 * @param stack pointer to a stack
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline bool stack_is_empty(const Stack* stack) {
    return deque_is_empty(stack);
}

/**
 * @brief Creates an iterator for the stack.
 *
 * @param stack pointer to a stack
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add(), set(), and remove()
 */
static inline Iterator* stack_iterator(const Stack* stack) {
    return deque_iterator(stack);
}

/**
 * @brief Checks whether two stacks are equal.
 *
 * Two stacks are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first stack
 *
 * @param stack first stack
 * @param other_stack second stack
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either stack is null
 */
static inline bool stack_equals(const Stack* stack, const Stack* other_stack) {
    return deque_equals(stack, other_stack);
}

/**
 * @brief Applies an action to each element of the stack.
 *
 * @param stack pointer to a stack
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if stack or action is null
 */
static inline void stack_for_each(Stack* stack, Consumer action) {
    deque_for_each(stack, action);
}

/**
 * @brief Removes all elements of the stack.
 *
 * @param stack pointer to a stack
 *
 * @exception NULL_POINTER_ERROR if stack is null
 *
 * @note this function calls the element destruct
 */
static inline void stack_clear(Stack* stack) {
    deque_clear(stack);
}

/**
 * @brief Checks whether an element is present in the stack.
 *
 * @param stack pointer to a stack
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline bool stack_contains(const Stack* stack, const void* element) {
    return deque_contains(stack, element);
}

/**
 * @brief Checks whether all elements of a collection are present in the stack.
 *
 * @param stack pointer to a stack
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
static inline bool stack_contains_all(const Stack* stack, Collection collection) {
    return deque_contains_all(stack, collection);
}

/**
 * @brief Creates a shallow copy of the stack.
 *
 * The new stack shares element pointers but has independent storage.
 *
 * @param stack pointer to a stack
 *
 * @return pointer to the newly created stack
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Stack* stack_clone(const Stack* stack) {
    return deque_clone(stack);
}

/**
 * @brief Returns a Collection view of the stack.
 *
 * @param stack pointer to a stack
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline Collection stack_to_collection(const Stack* stack) {
    return deque_to_collection(stack);
}

/**
 * @brief Returns a newly allocated array containing all elements of the stack.
 *
 * @param stack pointer to a stack
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
static inline void** stack_to_array(const Stack* stack) {
    return deque_to_array(stack);
}

/**
 * @brief Converts the stack to a string representation.
 *
 * @param stack pointer to a stack
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
static inline char* stack_to_string(const Stack* stack) {
    return deque_to_string(stack);
}

#endif