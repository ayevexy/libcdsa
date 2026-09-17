#define _GNU_SOURCE

#include "file.h"

#include "errors.h"
#include "memory.h"
#include "util/constraints.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

struct File {
    FILE* self;
};

static void file_set_error(int error) {
    switch (error) {
        #ifdef ENOENT
            case ENOENT:     set_error(FILE_NOT_FOUND_ERROR, "no such file or directory");            break;
        #endif

        #ifdef ENOTDIR
            case ENOTDIR:    set_error(FILE_NOT_FOUND_ERROR, "a path component is not a directory");  break;
        #endif

        #ifdef EEXIST
            case EEXIST:     set_error(FILE_ALREADY_EXISTS_ERROR, "file already exists");             break;
        #endif

        #ifdef EACCES
            case EACCES:     set_error(FILE_ACCESS_DENIED_ERROR, "permission denied");                break;
        #endif

        #ifdef EPERM
            case EPERM:      set_error(FILE_ACCESS_DENIED_ERROR, "operation not permitted");          break;
        #endif

        #ifdef ENOTEMPTY
            case ENOTEMPTY:  set_error(FILE_DIRECTORY_NOT_EMPTY_ERROR, "directory is not empty");     break;
        #endif

        #ifdef EIO
            case EIO:        set_error(FILE_INPUT_OUTPUT_ERROR, "input/output error");                break;
        #endif

        #ifdef EISDIR
            case EISDIR:     set_error(FILE_SYSTEM_ERROR, "path refers to a directory");              break;
        #endif

        default:             set_error(FILE_SYSTEM_ERROR, "%s", strerror(error));                     break;
    }
}

static File* file_new(FILE* handle) {
    if (!handle) {
        file_set_error(errno);
        return nullptr;
    }
    File* file = memory_try_alloc(sizeof(File));
    if (!file) {
        fclose(handle);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory to create the file object");
        return nullptr;
    }
    file->self = handle;
    return file;
}

File* file_create(const char* path) {
    if (require_non_null(path)) return nullptr;

    if (file_exists(path)) {
        set_error(FILE_ALREADY_EXISTS_ERROR, "file already exists");
        return nullptr;
    }
    return file_new(fopen(path, "w+"));
}

File* file_create_temporary() {
    return file_new(tmpfile());
}

static const char* file_open_modes(int modes) {
    const bool read = modes & FILE_READ;
    const bool write = modes & FILE_WRITE;
    const bool append = modes & FILE_APPEND;
    const bool truncate = modes & FILE_TRUNCATE;

    if (read && !write && !append) {
        return "r";
    }
    if (!read && write && !append) {
        return "w";
    }
    if (!read && write && append) {
        return "a";
    }
    if (read && write && !append && !truncate) {
        return "r+";
    }
    if (read && write && !append && truncate) {
        return "w+";
    }
    if (read && write && append) {
        return "a+";
    }
    set_error(ILLEGAL_ARGUMENT_ERROR, "unknown file open modes");
    return nullptr;
}

File* file_open(const char* path, FileOpenOption modes) {
    if (require_non_null(path)) return nullptr;
    const char* raw_modes = file_open_modes(modes);

    if (!raw_modes) {
        return nullptr;
    }
    return file_new(fopen(path, raw_modes));
}

void file_close(File* file) {
    if (require_non_null(file)) return;

    if (fclose(file->self) == EOF) {
        file_set_error(errno);
        return;
    }
    memory_dealloc(file);
}

bytes file_read(File* file, void* buffer, bytes size) {
    if (require_non_null(file, buffer)) return 0;

    const bytes read = fread(buffer, 1, size, file->self);

    if (read < size && ferror(file->self)) {
        file_set_error(errno);
    }
    return read;
}

int file_read_char(File* file) {
    if (require_non_null(file)) return -1;

    const int c = fgetc(file->self);

    if (c == EOF && ferror(file->self)) {
        file_set_error(errno);
    }
    return c;
}

String (file_read_line)(File* file, char* buffer, bytes size) {
    if (require_non_null(file, buffer)) return nullptr;

    if (!fgets(buffer, (int) size, file->self) && ferror(file->self)) {
        file_set_error(errno);
        return nullptr;
    }
    return string_new(buffer);
}

bytes file_write(File* file, const void* buffer, bytes size) {
    if (require_non_null(file, buffer)) return -1;

    const bytes written = fwrite(buffer, 1, size, file->self);

    if (written < size && ferror(file->self)) {
        file_set_error(errno);
    }
    return written;
}

int file_write_char(File* file, int character) {
    if (require_non_null(file)) return -1;

    const int c = fputc(character, file->self);

    if (c < 0) {
        file_set_error(errno);
    }
    return c;
}

int (file_write_string)(File* file, struct String string, ...) {
    if (require_non_null(file, string.data)) return -1;

    va_list parameters = {};
    va_start(parameters, string);

    const int bytes = vfprintf(file->self, string.data, parameters);
    va_end(parameters);

    if (bytes < 0) {
        file_set_error(errno);
    }
    return bytes;
}

int file_write_string_variadic(File* file, struct String string, va_list parameters) {
    if (require_non_null(file, string.data)) return -1;

    const int bytes = vfprintf(file->self, string.data, parameters);
    if (bytes < 0) {
        file_set_error(errno);
    }
    return bytes;
}

void file_seek(File* file, long offset, FileSeekOrigin origin) {
    if (require_non_null(file)) return;

    if (fseek(file->self, offset, origin) != 0) {
        file_set_error(errno);
    }
}

long file_position(File* file) {
    if (require_non_null(file)) return -1;

    const long position = ftell(file->self);

    if (position == -1L) {
        file_set_error(errno);
    }
    return position;
}

void file_rewind(File* file) {
    if (require_non_null(file)) return;

    if (fseek(file->self, 0L, SEEK_SET) != 0) {
        file_set_error(errno);
    }
}

bool file_at_end(File* file) {
    if (require_non_null(file)) return false;

    return feof(file->self);
}

void file_flush(File* file) {
    if (require_non_null(file)) return;

    if (fflush(file->self) == EOF) {
        file_set_error(errno);
    }
}

bool file_exists(const char* path) {
    if (require_non_null(path)) return false;

    FILE* file = fopen(path, "r");

    if (!file) {
        #ifdef ENOENT
        if (errno == ENOENT) {
            return false;
        }
        #endif
        file_set_error(errno);
        return false;
    }
    fclose(file);
    return true;
}

bytes file_size(File* file) {
    if (require_non_null(file)) return -1;

    const long position = ftell(file->self);
    if (position == -1L) {
        file_set_error(errno);
        return -1;
    }
    if (fseek(file->self, 0, SEEK_END) != 0) {
        file_set_error(errno);
        return -1;
    }

    const long size = ftell(file->self);
    if (size == -1L) {
        file_set_error(errno);
        fseek(file->self, position, SEEK_SET);
        return -1;
    }
    if (fseek(file->self, position, SEEK_SET) != 0) {
        file_set_error(errno);
        return -1;
    }
    return size;
}

#ifdef __linux

#include <fcntl.h>
#include <sys/stat.h>

FileInfo file_info(const char* path) {
    if (require_non_null(path)) return (FileInfo) {};

    struct statx info;

    if (statx(AT_FDCWD, path, 0,
        STATX_TYPE | STATX_SIZE | STATX_BTIME | STATX_MTIME | STATX_ATIME, &info) == -1)
    {
        file_set_error(errno);
        return (FileInfo) {};
    }

    return (FileInfo) {
        .size = info.stx_size,
        .is_regular = S_ISREG(info.stx_mode),
        .is_directory = S_ISDIR(info.stx_mode),
        .is_symbolic_link = S_ISLNK(info.stx_mode),
        .creation_time = info.stx_btime.tv_sec,
        .modified_time = info.stx_mtime.tv_sec,
        .access_time = info.stx_atime.tv_sec
    };
}

#endif

void file_move(const char* old_path, const char* new_path) {
    if (require_non_null(old_path, new_path)) return;

    if (rename(old_path, new_path) == -1) {
        file_set_error(errno);
    }
}

void file_delete(const char* path) {
    if (require_non_null(path)) return;

    if (remove(path) == -1) {
        file_set_error(errno);
    }
}

bool file_delete_if_exists(const char* path) {
    if (require_non_null(path)) return false;

    if (remove(path) == 0) {
        return true;
    }
    #ifdef ENOENT
    if (errno == ENOENT) {
        return false;
    }
    #endif
    file_set_error(errno);
    return false;
}

const char* file_system_error_to_string(uint8 error) {
    static const char* error_strings[] = {
        "FILE_SYSTEM_ERROR",
        "FILE_INPUT_OUTPUT_ERROR",
        "FILE_NOT_FOUND_ERROR",
        "FILE_ALREADY_EXISTS_ERROR",
        "FILE_ACCESS_DENIED_ERROR",
        "FILE_DIRECTORY_NOT_EMPTY_ERROR"
    };
    return error < sizeof(error_strings) / sizeof(error_strings[0])
        ? error_strings[error]
        : "UNKNOWN_ERROR";
}