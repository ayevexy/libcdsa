#include "sync.h"

#ifdef __linux__

#include "memory.h"
#include "util/constraints.h"

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

struct Monitor {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

Monitor* monitor_new(void) {
    Monitor* monitor = memory_try_alloc(sizeof(Monitor));

    if (!monitor) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'monitor'");
        return nullptr;
    }

    int status = pthread_mutex_init(&monitor->mutex, nullptr);

    if (status != 0) {
        memory_dealloc(monitor);
        set_error(SYNCHRONIZATION_ERROR, "failed to initialize monitor");
        return nullptr;
    }

    status = pthread_cond_init(&monitor->condition, nullptr);

    if (status != 0) {
        pthread_mutex_destroy(&monitor->mutex);
        memory_dealloc(monitor);
        set_error(SYNCHRONIZATION_ERROR, "failed to initialize monitor");
        return nullptr;
    }
    return monitor;
}

void monitor_destroy(Monitor** monitor_pointer) {
    if (require_non_null(monitor_pointer, *monitor_pointer)) return;
    Monitor* monitor = *monitor_pointer;

    const int condition_status = pthread_cond_destroy(&monitor->condition);
    const int mutex_status = pthread_mutex_destroy(&monitor->mutex);

    if (condition_status != 0 || mutex_status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to destroy monitor");
        return;
    }
    memory_dealloc(monitor);
    *monitor_pointer = nullptr;
}

void monitor_lock(Monitor* monitor) {
    if (require_non_null(monitor)) return;

    const int status = pthread_mutex_lock(&monitor->mutex);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to lock monitor");
    }
}

bool monitor_try_lock(Monitor* monitor) {
    if (require_non_null(monitor)) return false;

    const int status = pthread_mutex_trylock(&monitor->mutex);

    if (status == 0) {
        return true;
    }
    if (status == EBUSY) {
        return false;
    }
    set_error(SYNCHRONIZATION_ERROR, "failed to try-lock monitor");
    return false;
}

void monitor_unlock(Monitor* monitor) {
    if (require_non_null(monitor)) return;

    const int status = pthread_mutex_unlock(&monitor->mutex);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to unlock monitor");
    }
}

void monitor_wait(Monitor* monitor) {
    if (require_non_null(monitor)) return;

    const int status = pthread_cond_wait(&monitor->condition, &monitor->mutex);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to wait on monitor");
    }
}

void monitor_notify(Monitor* monitor) {
    if (require_non_null(monitor)) return;

    const int status = pthread_cond_signal(&monitor->condition);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to notify monitor");
    }
}

void monitor_notify_all(Monitor* monitor) {
    if (require_non_null(monitor)) return;

    const int status = pthread_cond_broadcast(&monitor->condition);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to notify all threads waiting on monitor");
    }
}

struct Semaphore {
    sem_t semaphore;
};

Semaphore* semaphore_new(int permits) {
    if (permits < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "number of permits can't be negative");
        return nullptr;
    }
    Semaphore* semaphore = memory_try_alloc(sizeof(Semaphore));

    if (!semaphore) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'semaphore'");
        return nullptr;
    }

    const int status = sem_init(&semaphore->semaphore, 0, permits);

    if (status != 0) {
        memory_dealloc(semaphore);
        set_error(SYNCHRONIZATION_ERROR, "failed to initialize semaphore");
        return nullptr;
    }
    return semaphore;
}

void semaphore_destroy(Semaphore** semaphore_pointer) {
    if (require_non_null(semaphore_pointer, *semaphore_pointer)) return;
    Semaphore* semaphore = *semaphore_pointer;

    const int status = sem_destroy(&semaphore->semaphore);
    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to destroy semaphore");
        return;
    }
    memory_dealloc(semaphore);
    *semaphore_pointer = nullptr;
}

void semaphore_acquire(Semaphore* semaphore) {
    if (require_non_null(semaphore)) return;

    while (sem_wait(&semaphore->semaphore) != 0) {
        if (errno != EINTR) {
            set_error(SYNCHRONIZATION_ERROR, "failed to acquire semaphore");
            return;
        }
    }
}

bool semaphore_try_acquire(Semaphore* semaphore) {
    if (require_non_null(semaphore)) return false;

    const int status = sem_trywait(&semaphore->semaphore);

    if (status == 0) {
        return true;
    }
    if (errno == EAGAIN) {
        return false;
    }
    set_error(SYNCHRONIZATION_ERROR, "failed to try-acquire semaphore");
    return false;
}

void semaphore_release(Semaphore* semaphore) {
    if (require_non_null(semaphore)) return;

    const int status = sem_post(&semaphore->semaphore);

    if (status != 0) {
        set_error(SYNCHRONIZATION_ERROR, "failed to release semaphore");
    }
}

const char* synchronization_error_to_string(uint8 error) {
    static const char* error_strings[] = {
        "SYNCHRONIZATION_ERROR",
    };
    return error < sizeof(error_strings) / sizeof(error_strings[0])
        ? error_strings[error]
        : "UNKNOWN_ERROR";
}

#endif
