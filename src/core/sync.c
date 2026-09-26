#include "sync.h"

#ifdef __linux__

#include "memory.h"
#include "util/constraints.h"

#include <errno.h>
#include <pthread.h>

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

const char* synchronization_error_to_string(uint8 error) {
    static const char* error_strings[] = {
        "SYNCHRONIZATION_ERROR",
    };
    return error < sizeof(error_strings) / sizeof(error_strings[0])
        ? error_strings[error]
        : "UNKNOWN_ERROR";
}

#endif
