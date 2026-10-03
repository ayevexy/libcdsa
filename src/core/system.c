#include "system.h"

#include "file.h"
#include "constraints.h"

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct File {
    FILE* self;
};

static File* system_input_stream;

static File* system_output_stream;

static File* system_error_stream;

extern int file_write_string_variadic(File*, struct String, va_list);

File* system_input(void) {
    static File system_standard_input_stream;

    if (!system_input_stream) {
        system_standard_input_stream.self = stdin;
        system_input_stream = &system_standard_input_stream;
    }
    return system_input_stream;
}

void system_change_input(File* input) {
    if (require_non_null(input)) return;
    system_input_stream = input;
}

File* system_output(void) {
    static File system_standard_output_stream;

    if (!system_output_stream) {
        system_standard_output_stream.self = stdout;
        system_output_stream = &system_standard_output_stream;
    }
    return system_output_stream;
}

void system_change_output(File* output) {
    if (require_non_null(output)) return;
    system_output_stream = output;
}

File* system_error(void) {
    static File system_standard_error_stream;

    if (!system_error_stream) {
        system_standard_error_stream.self = stderr;
        system_error_stream = &system_standard_error_stream;
    }
    return system_error_stream;
}

void system_change_error(File* error) {
    if (require_non_null(error)) return;
    system_error_stream = error;
}

char system_read(void) {
    return file_read_char(system_input());
}

String (system_read_line)(char* buffer, bytes size) {
    return (file_read_line)(system_input(), buffer, size);
}

void (system_write)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    file_write_string_variadic(system_output(), string, parameters);
    va_end(parameters);
}

void (system_write_line)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    file_write_string_variadic(system_output(), string, parameters);
    va_end(parameters);

    file_write_char(system_output(), '\n');
}

void (system_write_error)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    file_write_string_variadic(system_error(), string, parameters);
    va_end(parameters);
}

void (system_write_error_line)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    file_write_string_variadic(system_error(), string, parameters);
    va_end(parameters);

    file_write_char(system_error(), '\n');
}

const char* system_get_environment_variable(const char* name) {
    if (require_non_null(name)) return nullptr;
    return getenv(name);
}

#ifdef __linux__

void system_set_environment_variable(const char* name, const char* value) {
    if (require_non_null(name)) return;
    if (setenv(name, value, 1) == -1) {
        switch (errno) {
            case EINVAL:  set_error(ILLEGAL_ARGUMENT_ERROR, "invalid environment variable name");  break;
            case ENOMEM:  set_error(MEMORY_ALLOCATION_ERROR, "insufficient memory");               break;
            default:      unreachable();
        }
    }
}

void system_remove_environment_variable(const char* name) {
    if (require_non_null(name)) return;
    if (unsetenv(name) == 0) {
        return;
    }
    if (errno == EINVAL) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "invalid environment variable name");
    }
}

#endif

const char* system_platform_name(void) {
    #if defined(__linux__)
        return "Linux";
    #else
        return "Unknown";
    #endif
}

#ifdef __linux__
#include <sys/utsname.h>
#endif

const char* system_platform_version(void) {
    #ifdef __linux__
        static struct utsname info;

        if (uname(&info) != 0) {
            return nullptr;
        }
        return info.release;
    #else
        return "unknown";
    #endif
}

const char* system_platform_architecture(void) {
    #if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
    #elif defined(__aarch64__) || defined(_M_ARM64)
        return "aarch64";
    #elif defined(__i386__) || defined(_M_IX86)
        return "x86";
    #elif defined(__arm__) || defined(_M_ARM)
        return "arm";
    #else
        return "unknown";
    #endif
}

#ifdef __linux__

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

const Error PROCESS_CREATION_ERROR      = ERROR("PROCESS_CREATION_ERROR");
const Error PROCESS_EXECUTION_ERROR     = ERROR("PROCESS_EXECUTION_ERROR");
const Error PROCESS_NOT_FOUND_ERROR     = ERROR("PROCESS_NOT_FOUND_ERROR");
const Error PROCESS_ACCESS_DENIED_ERROR = ERROR("PROCESS_ACCESS_DENIED_ERROR");
const Error PROCESS_INTERRUPTED_ERROR   = ERROR("PROCESS_INTERRUPTED_ERROR");

static void system_process_set_creation_error(int error) {
    switch (error) {
        case EAGAIN:  set_error(PROCESS_CREATION_ERROR, "unable to create process due to limited resources");          break;
        case ENOMEM:  set_error(PROCESS_CREATION_ERROR, "unable to create process due to limited resources");          break;
        case ENOSYS:  set_error(PROCESS_CREATION_ERROR, "process creation is not supported by the operating system");  break;
        default:      set_error(PROCESS_CREATION_ERROR, "%s", strerror(error));
    }
}

static void system_process_set_execution_error(int error) {
    switch (error) {
        case EACCES:   set_error(PROCESS_ACCESS_DENIED_ERROR, "permission denied");              break;
        case ENOENT:   set_error(PROCESS_EXECUTION_ERROR, "executable not found");               break;
        case ENOTDIR:  set_error(PROCESS_EXECUTION_ERROR, "path component is not a directory");  break;
        case ELOOP:    set_error(PROCESS_EXECUTION_ERROR, "too many symbolic links");            break;
        case ENOEXEC:  set_error(PROCESS_EXECUTION_ERROR, "invalid executable format");          break;
        case ENOMEM:   set_error(PROCESS_EXECUTION_ERROR, "insufficient memory");                break;
        case ETXTBSY:  set_error(PROCESS_EXECUTION_ERROR, "executable is being used");           break;
        default:       set_error(PROCESS_EXECUTION_ERROR, "%s", strerror(error));
    }
}

intptr (system_process_create)(int count, ...) {
    if (count == 0) {
        const pid_t process_id = fork();

        if (process_id == -1) {
            system_process_set_creation_error(errno);
            return -1;
        }
        return process_id;
    }

    int error_pipe[2];

    if (pipe(error_pipe) == -1) {
        set_error(PROCESS_CREATION_ERROR, "pipe creation for process failed");
        return -1;
    }
    fcntl(error_pipe[1], F_SETFD, FD_CLOEXEC);

    const pid_t process_id = fork();

    if (process_id == -1) {
        const int error = errno;
        close(error_pipe[0]);
        close(error_pipe[1]);
        system_process_set_creation_error(error);
        return -1;
    }
    if (process_id == 0) {
        close(error_pipe[0]);

        va_list parameters = {};
        va_start(parameters, count);

        char* arguments[count + 1];
        for (int i = 0; i < count; i++) {
            arguments[i] = va_arg(parameters, char*);
        }
        arguments[count] = nullptr;
        va_end(parameters);

        execvp(arguments[0], arguments);

        const int error = errno;
        const isize n = write(error_pipe[1], &error, sizeof(error));

        close(error_pipe[1]);
        _exit(n == sizeof(error) ? 127 : 126);
    }
    close(error_pipe[1]);

    int error;
    const isize n = read(error_pipe[0], &error, sizeof(error));

    if (n == sizeof(error)) {
        system_process_set_execution_error(error);
    } else if (n == -1) {
        const int read_error = errno;
        set_error(PROCESS_CREATION_ERROR, "%s", strerror(read_error));
    }

    close(error_pipe[0]);
    return process_id;
}

int system_process_wait(intptr process_id) {
    int status;
    if (waitpid(process_id, &status, 0) == -1) {
        switch (errno) {
            case ECHILD:  set_error(PROCESS_NOT_FOUND_ERROR, "no child process");                 break;
            case EINTR:   set_error(PROCESS_INTERRUPTED_ERROR, "process operation interrupted");  break;
            default:      unreachable();
        }
        return -1;
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return -WTERMSIG(status);
    }
    return -1;
}

int system_process_wait_timeout(intptr process_id, uint64 timeout, bool* timed_out) {
    int status;
    uint64 elapsed = 0;

    if (timed_out) {
        *timed_out = false;
    }
    while (elapsed <= timeout) {
        const pid_t result = waitpid(process_id, &status, WNOHANG);

        if (result == (pid_t) process_id) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return -WTERMSIG(status);
            }
            return -1;
        }
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == ECHILD) {
                set_error(PROCESS_NOT_FOUND_ERROR, "no child process");
            }
            return -1;
        }

        if (elapsed == timeout) {
            break;
        }
        uint64 delay = timeout - elapsed;
        if (delay > 10) {
            delay = 10;
        }
        const struct timespec sleep_time = {
            .tv_sec = delay / 1000,
            .tv_nsec = (delay % 1000) * 1000000
        };
        nanosleep(&sleep_time, NULL);
        elapsed += delay;
    }

    if (timed_out) {
        *timed_out = true;
    }
    return -1;
}

intptr system_process_current(void) {
    return getpid();
}

intptr system_process_parent(void) {
    return getppid();
}

bool system_process_is_alive(intptr process_id) {
    if (kill(process_id, 0) == -1) {
        switch (errno) {
            case ESRCH: return false;
            case EPERM: return true;
            default: unreachable();
        }
    }
    return true;
}

void system_process_signal(intptr process_id, int signum) {
    if (kill(process_id, signum) == -1) {
        switch (errno) {
            case EINVAL:  set_error(ILLEGAL_ARGUMENT_ERROR, "invalid signal number");           break;
            case ESRCH:   set_error(PROCESS_NOT_FOUND_ERROR, "no process with that id found");  break;
            case EPERM:   set_error(PROCESS_ACCESS_DENIED_ERROR, "permission denied");          break;
            default:      unreachable();
        }
    }
}

void system_process_suspend(intptr process_id) {
    system_process_signal(process_id, SIGSTOP);
}

void system_process_resume(intptr process_id) {
    system_process_signal(process_id, SIGCONT);
}

void system_process_terminate(intptr process_id) {
    system_process_signal(process_id, SIGTERM);
}

void system_process_kill(intptr process_id) {
    system_process_signal(process_id, SIGKILL);
}

#include <pthread.h>

const Error THREAD_CREATION_ERROR      = ERROR("THREAD_CREATION_ERROR");
const Error THREAD_NOT_FOUND_ERROR     = ERROR("THREAD_NOT_FOUND_ERROR");
const Error THREAD_ILLEGAL_STATE_ERROR = ERROR("THREAD_ILLEGAL_STATE_ERROR");
const Error THREAD_DEADLOCK_ERROR      = ERROR("THREAD_DEADLOCK_ERROR");

intptr (system_thread_create)(void* (*routine)(void*), void* argument) {
    if (require_non_null(routine)) return -1;

    pthread_t thread_id;
    const int status = pthread_create(&thread_id, nullptr, routine, argument);

    if (status != 0) {
        switch (status) {
            case EAGAIN: set_error(THREAD_CREATION_ERROR, "unable to create thread due to limited resources"); break;
            default: set_error(THREAD_CREATION_ERROR, "%s", strerror(status)); break;
        }
        return -1;
    }
    return thread_id;
}

void* system_thread_join(intptr thread_id) {
    void* result = nullptr;
    const int status = pthread_join(thread_id, &result);
    if (status != 0) {
        switch (status) {
            case ESRCH: set_error(THREAD_NOT_FOUND_ERROR, "no thread with that id found"); break;
            case EINVAL: set_error(THREAD_ILLEGAL_STATE_ERROR, "thread is not joinable or is already being joined"); break;
            case EDEADLK: set_error(THREAD_DEADLOCK_ERROR, "thread cannot be joined without causing deadlock"); break;
            default: unreachable();
        }
    }
    return result;
}

void system_thread_detach(intptr thread_id) {
    const int status = pthread_detach(thread_id);
    if (status != 0) {
        switch (status) {
            case ESRCH: set_error(THREAD_NOT_FOUND_ERROR, "no thread with that id found"); break;
            case EINVAL: set_error(THREAD_ILLEGAL_STATE_ERROR, "thread is not joinable"); break;
            default: unreachable();
        }
    }
}

void system_thread_interrupt(intptr thread_id) {
    const int status = pthread_cancel(thread_id);
    if (status != 0) {
        switch (status) {
            case ESRCH: set_error(THREAD_NOT_FOUND_ERROR, "no thread with that id found"); break;
            default: unreachable();
        }
    }
}

void system_thread_yield(void) {
    sched_yield();
}

intptr system_thread_current(void) {
    return pthread_self();
}

void system_thread_exit(void* result) {
    pthread_exit(result);
}

void system_thread_sleep(uint64 milliseconds) {
    struct timespec remaining = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000
    };
    while (nanosleep(&remaining, &remaining) == -1 && errno == EINTR) {}
}

#endif

int system_execute(const char* command) {
    if (require_non_null(command)) return -1;

    const int status = system(command);
    if (status == -1) {
        set_error(PROCESS_EXECUTION_ERROR, "%s", strerror(errno));
        return -1;
    }
#ifdef __linux__
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return -WTERMSIG(status);
    }
    set_error(PROCESS_EXECUTION_ERROR, "unknown process termination status");
    return -1;
#else
    return status;
#endif
}

uint64 system_random(void) {
    return rand();
}

uint64 system_current_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);

    return (uint64) time.tv_sec * 1000
         + (uint64) time.tv_nsec / 1000000;
}

uint64 system_elapsed_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);

    return (uint64) time.tv_sec * 1000000000
         + (uint64) time.tv_nsec;
}

void system_exit(int status) {
    exit(status);
}