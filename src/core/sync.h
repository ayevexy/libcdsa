#ifndef LIBCDSA_SYNC_H
#define LIBCDSA_SYNC_H

#ifdef __linux__

#include "types.h"
#include "errors.h"

/**
 * @brief Synchronization error constant.
 */
extern const Error SYNCHRONIZATION_ERROR;

/**
 * @brief Represents a monitor.
 */
typedef struct Monitor Monitor;

/**
 * @brief Creates a new monitor.
 *
 * @return A pointer to the newly created monitor, or nullptr on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed to allocate memory
 * @exception SYNCHRONIZATION_ERROR If failed to initialize the monitor
 */
Monitor* monitor_new(void);

/**
 * @brief Destroys a monitor.
 *
 * @param monitor_pointer pointer to a monitor pointer
 *
 * @exception NULL_POINTER_ERROR if monitor_pointer or *monitor_pointer is null
 * @exception SYNCHRONIZATION_ERROR If failed to destroy the monitor
 *
 * @post *monitor_pointer == nullptr
 */
void monitor_destroy(Monitor** monitor_pointer);

/**
 * @brief Locks the monitor.
 *
 * @param monitor the monitor
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If the monitor could not be locked
 */
void monitor_lock(Monitor* monitor);

/**
 * @brief Attempts to lock the monitor.
 *
 * @param monitor the monitor
 *
 * @return true if the monitor was successfully locked, false otherwise
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If an error occurs
 */
bool monitor_try_lock(Monitor* monitor);

/**
 * @brief Unlocks the monitor.
 *
 * @param monitor the monitor
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If the monitor could not be unlocked
 */
void monitor_unlock(Monitor* monitor);

/**
 * @brief Waits for a notification from another thread.
 *
 * @param monitor the monitor
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If the thread could not wait on the monitor
 */
void monitor_wait(Monitor* monitor);

/**
 * @brief Notifies one thread waiting on the monitor.
 *
 * @param monitor the monitor
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If the notification could not be sent
 */
void monitor_notify(Monitor* monitor);

/**
 * @brief Notifies all threads waiting on the monitor.
 *
 * @param monitor the monitor
 *
 * @exception NULL_POINTER_ERROR If monitor is null
 * @exception SYNCHRONIZATION_ERROR If the notification could not be sent
 */
void monitor_notify_all(Monitor* monitor);

/**
 * @brief Represents a semaphore.
 */
typedef struct Semaphore Semaphore;

/**
 * @brief Creates a new semaphore.
 *
 * @param permits initial number of available permits
 *
 * @return A pointer to the newly created semaphore, or nullptr on failure
 *
 * @exception ILLEGAL_ARGUMENT_ERROR If permits is negative
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 * @exception SYNCHRONIZATION_ERROR If the semaphore cannot be initialized
 */
Semaphore* semaphore_new(int permits);

/**
 * @brief Destroys a semaphore.
 *
 * @param semaphore_pointer pointer to a semaphore pointer
 *
 * @exception NULL_POINTER_ERROR if semaphore_pointer or *semaphore_pointer is null
 * @exception SYNCHRONIZATION_ERROR If failed to destroy the semaphore
 *
 * @post *semaphore_pointer == nullptr
 *
 * @warning The semaphore must not have threads blocked on it.
 */
void semaphore_destroy(Semaphore** semaphore_pointer);

/**
 * @brief Acquires a permit from the semaphore, blocking until one is available.
 *
 * @param semaphore the semaphore
 *
 * @exception NULL_POINTER_ERROR If semaphore is null
 * @exception SYNCHRONIZATION_ERROR If the semaphore cannot be acquired
 */
void semaphore_acquire(Semaphore* semaphore);

/**
 * @brief Attempts to acquire a permit from the semaphore without blocking
 *
 * @param semaphore the semaphore
 *
 * @return true if a permit was acquired, or false if no permit is available
 *
 * @exception NULL_POINTER_ERROR If semaphore is null
 * @exception SYNCHRONIZATION_ERROR If the semaphore cannot be queried
 */
bool semaphore_try_acquire(Semaphore* semaphore);

/**
 * @brief Releases a permit from the semaphore.
 *
 * @param semaphore the semaphore
 *
 * @exception NULL_POINTER_ERROR If semaphore is null
 * @exception SYNCHRONIZATION_ERROR If the semaphore cannot be released
 */
void semaphore_release(Semaphore* semaphore);

#endif

#endif