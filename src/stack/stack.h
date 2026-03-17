#ifndef STACK_H
#define STACK_H

#include "deque/deque.h"

/**
 * A Stack is a generic LIFO (Last-In-First-Out) data structure built on top of a Deque.
 * Elements are added and removed from the end of the stack only.
 *
 * Internally, a Stack is simply a type alias for a Deque and inherits all its dynamic resizing
 * and generic type capabilities.
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
 * @brief Provides a reasonable default StackOptions configuration.
 *
 * Alias of DEFAULT_DEQUE_OPTIONS. Optional overrides can be provided.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_STACK_OPTIONS DEFAULT_DEQUE_OPTIONS

/**
 * @brief Creates a new empty Stack using the specified options.
 *
 * @param options pointer to a StackOptions defining the stack configuration
 *
 * @return pointer to a newly created Stack on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Stack* stack_new(const StackOptions* options) {
    return deque_new(options);
}

/**
 * @brief Creates a new Stack containing all elements of the given collection.
 *
 * @param collection a Collection containing elements to add
 * @param options pointer to a StackOptions defining configuration
 *
 * @return pointer to a newly created Stack on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Stack* stack_from(Collection collection, const StackOptions* options) {
    return deque_from(collection, options);
}

/**
 * @brief Destroys an existing Stack and optionally its elements using the destruct function.
 *
 * @param stack pointer to a Stack pointer
 *
 * @post *stack == nullptr
 *
 * @exception NULL_POINTER_ERROR if stack or *stack is null
 */
static inline void stack_destroy(Stack** stack) {
    deque_destroy(stack);
}

/**
 * @brief Sets the destructor function used for elements in the Stack.
 *
 * @param stack pointer to a Stack
 * @param destructor function to free elements
 */
static inline void stack_set_destructor(Stack* stack, void (*destructor)(void*)) {
    deque_set_destructor(stack, destructor);
}

/**
 * @brief Pushes an element onto the top of the Stack.
 *
 * @param stack pointer to a Stack
 * @param element pointer to the element to push
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
static inline void stack_push(Stack* stack, const void* element) {
    deque_add_last(stack, element);
}

/**
 * @brief Pushes all elements of a collection onto the top of the Stack.
 *
 * @param stack pointer to a Stack
 * @param collection Collection of elements to push
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if iterator allocation fails
 */
static inline void stack_push_all(Stack* stack, Collection collection) {
    deque_add_all_last(stack, collection);
}

/**
 * @brief Retrieves the element at the top of the Stack without removing it.
 *
 * @param stack pointer to a Stack
 *
 * @return pointer to the top element
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception NO_SUCH_ELEMENT_ERROR if the stack is empty
 */
static inline void* stack_peek(const Stack* stack) {
    return deque_get_last(stack);
}

/**
 * @brief Removes and returns the element at the top of the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception NO_SUCH_ELEMENT_ERROR if the stack is empty
 */
static inline void* stack_pop(Stack* stack) {
    return deque_remove_last(stack);
}

/**
 * @brief Returns the current number of elements in the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return number of elements
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline int stack_size(const Stack* stack) {
    return deque_size(stack);
}

/**
 * @brief Returns the current capacity of the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return capacity of the stack
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline int stack_capacity(const Stack* stack) {
    return deque_capacity(stack);
}

/**
 * @brief Checks whether the Stack is empty.
 *
 * @param stack pointer to a Stack
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline bool stack_is_empty(const Stack* stack) {
    return deque_is_empty(stack);
}

/**
 * @brief Returns an iterator over the elements in the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return pointer to an Iterator
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline Iterator* stack_iterator(const Stack* stack) {
    return deque_iterator(stack);
}

/**
 * @brief Checks whether two Stacks are equal.
 *
 * Two stacks are equal if they have the same size and all elements are equal
 * according to the equals function of the first stack.
 *
 * @param stack pointer to a Stack
 * @param other pointer to another Stack
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack or other is null
 */
static inline bool stack_equals(const Stack* stack, const Stack* other) {
    return deque_equals(stack, other);
}

/**
 * @brief Performs an action for each element of the Stack.
 *
 * @param stack pointer to a Stack
 * @param action function to apply to each element
 *
 * @exception NULL_POINTER_ERROR if stack or action is null
 */
static inline void stack_for_each(Stack* stack, Consumer action) {
    deque_for_each(stack, action);
}

/**
 * @brief Removes all elements from the Stack, optionally destructing them.
 *
 * @param stack pointer to a Stack
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline void stack_clear(Stack* stack) {
    deque_clear(stack);
}

/**
 * @brief Checks whether the Stack contains the specified element.
 *
 * @param stack pointer to a Stack
 * @param element pointer to the element to search
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline bool stack_contains(const Stack* stack, const void* element) {
    return deque_contains(stack, element);
}

/**
 * @brief Checks whether the Stack contains all elements of a collection.
 *
 * @param stack pointer to a Stack
 * @param collection Collection of elements to check
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if iterator allocation fails
 */
static inline bool stack_contains_all(const Stack* stack, Collection collection) {
    return deque_contains_all(stack, collection);
}

/**
 * @brief Creates a shallow copy of the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return pointer to a new Stack containing the same element pointers
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline Stack* stack_clone(const Stack* stack) {
    return deque_clone(stack);
}

/**
 * @brief Returns a Collection view of the Stack.
 *
 * @param stack pointer to a Stack
 *
 * @return Collection representation
 *
 * @exception NULL_POINTER_ERROR if stack is null
 */
static inline Collection stack_to_collection(const Stack* stack) {
    return deque_to_collection(stack);
}

/**
 * @brief Converts the Stack into a newly allocated array.
 *
 * @param stack pointer to a Stack
 *
 * @return newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline void** stack_to_array(const Stack* stack) {
    return deque_to_array(stack);
}

/**
 * @brief Converts the Stack into a string representation.
 *
 * @param stack pointer to a Stack
 *
 * @return newly allocated null-terminated string, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if stack is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline char* stack_to_string(const Stack* stack) {
    return deque_to_string(stack);
}

#endif