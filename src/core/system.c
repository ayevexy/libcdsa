#include "system.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void* system_input = nullptr;

void* system_output = nullptr;

void* system_error = nullptr;

static void* system_input_stream() {
    if (!system_input) {
        system_input = stdin;
    }
    return system_input;
}

static void* system_output_stream() {
    if (!system_output) {
        system_output = stdout;
    }
    return system_output;
}

static void* system_error_stream() {
    if (!system_error) {
        system_error = stderr;
    }
    return system_error;
}

char system_read() {
    return fgetc(system_input_stream());
}

String (system_read_line)(char* buffer, bytes size) {
    fgets(buffer, size, system_input_stream());
    return string_new(buffer);
}

void (system_write)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    vfprintf(system_output_stream(), string.data, parameters);
    va_end(parameters);
}

void (system_write_line)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    vfprintf(system_output_stream(), string.data, parameters);
    va_end(parameters);

    fprintf(system_output_stream(), "\n");
}

void (system_write_error)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    vfprintf(system_error_stream(), string.data, parameters);
    va_end(parameters);
}

void (system_write_error_line)(struct String string, ...) {
    va_list parameters = {};
    va_start(parameters, string);

    vfprintf(system_error_stream(), string.data, parameters);
    va_end(parameters);

    fprintf(system_error_stream(), "\n");
}

const char* system_get_environment_variable(const char* name) {
    return getenv(name);
}

void (system_set_environment_variable)(const char* name, const char* value) {
    setenv(name, value, 1);
}

void (system_remove_environment_variable)(const char* name) {
    unsetenv(name);
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

int system_execute(const char* command) {
    return system(command);
}

void system_exit(int status) {
    exit(status);
}