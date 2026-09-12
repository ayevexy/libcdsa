#include "system.h"

#include "util/constraints.h"
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

File* system_input() {
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

File* system_output() {
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

File* system_error() {
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

char system_read() {
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

void (system_set_environment_variable)(const char* name, const char* value) {
    if (require_non_null(name)) return;
    setenv(name, value, 1);
    if (errno == 0) {
        return;
    }
    switch (errno) {
        #ifdef EINVAL
            case EINVAL:  set_error(ILLEGAL_ARGUMENT_ERROR, "invalid environment variable name");  break;
        #endif

        #ifdef ENOMEM
            case ENOMEM:  set_error(MEMORY_ALLOCATION_ERROR, "insufficient memory");               break;
        #endif
            default:      set_error(UNKNOWN_ERROR, "%s", strerror(errno));                         break;
    }
}

void (system_remove_environment_variable)(const char* name) {
    if (require_non_null(name)) return;
    unsetenv(name);
    if (errno == 0) {
        return;
    }
    #ifdef EINVAL
        if (errno == EINVAL) {
            set_error(ILLEGAL_ARGUMENT_ERROR, "invalid environment variable name");
            return;
        }
    #endif
    set_error(UNKNOWN_ERROR, "%s", strerror(errno));
}

const char* system_platform_name() {
    #if defined(__linux__)
        return "Linux";
    #else
        return "Unknown";
    #endif
}

#ifdef __linux__
#include <sys/utsname.h>
#endif

const char* system_platform_version() {
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

const char* system_platform_architecture() {
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

uintptr (system_process_create)(int count, ...) {
    pid_t process_id = fork();
    if (process_id == -1) {
        perror("fork() failed");
        return -1;
    }
    if (process_id == 0 && count > 0) {
        va_list parameters = {};
        va_start(parameters, count);

        char* arguments[count + 1];
        for (int i = 0; i < count; i++) {
            arguments[i] = va_arg(parameters, char*);
        }
        arguments[count] = nullptr;
        va_end(parameters);

        execvp(arguments[0], arguments);
        perror("execvp() failed");
        _exit(127);
    }
    return process_id;
}

int system_process_wait(uintptr process_id) {
    int status;
    if (waitpid(process_id, &status, 0) == -1) {
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

int system_process_wait_timeout(uintptr process_id, uint64 timeout, bool* timed_out) {
    int status;
    uint64 elapsed = 0;
    if (timed_out) *timed_out = false;

    while (elapsed < timeout) {
        pid_t result = waitpid(process_id, &status, WNOHANG);

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
            return -1;
        }
        struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 10 * 1000 * 1000
        };
        nanosleep(&delay, NULL);
        elapsed += 10;
    }
    if (timed_out) *timed_out = true;
    return -1;
}

bool system_process_is_alive(uintptr process_id) {
    if (!kill(process_id, 0)) {
        return true;
    }
    if (errno == EPERM) {
        return true;
    }
    return false;
}

bool system_process_signal(uintptr process_id, int signum) {
    return !kill(process_id, signum);
}

bool system_process_suspend(uintptr process_id) {
    return !kill(process_id, SIGSTOP);
}

bool system_process_resume(uintptr process_id) {
    return !kill(process_id, SIGCONT);
}

bool system_process_terminate(uintptr process_id) {
    return !kill(process_id, SIGTERM);
}

bool system_process_kill(uintptr process_id) {
    return !kill(process_id, SIGKILL);
}

#endif

int system_execute(const char* command) {
    return system(command);
}

void system_exit(int status) {
    exit(status);
}