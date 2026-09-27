#ifndef LIBCDSA_FILE_H
#define LIBCDSA_FILE_H

#include "types.h"
#include "errors.h"
#include "array.h"
#include "string.h"

/**
 * @brief Represents a file.
 */
typedef struct File File;

/**
 * @brief Enumeration of possible file system errors.
 */
typedef enum : Error {
    FILE_SYSTEM_ERROR = ERROR_BASE(FILE_SYSTEM_ERROR_CATEGORY),
    FILE_INPUT_OUTPUT_ERROR,
    FILE_NOT_FOUND_ERROR,
    FILE_ALREADY_EXISTS_ERROR,
    FILE_ACCESS_DENIED_ERROR,
    FILE_DIRECTORY_NOT_EMPTY_ERROR
} FileSystemError;

/**
 * @brief Creates a file.
 *
 * @param path the file path
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_NOT_FOUND_ERROR if a path component does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the path is denied
 * @exception FILE_ALREADY_EXISTS_ERROR if the file already exists
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_create(const char* path);

#ifdef __linux__

/**
 * @brief Creates a directory.
 *
 * @param path the directory path
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_NOT_FOUND_ERROR if a path component does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the path is denied
 * @exception FILE_ALREADY_EXISTS_ERROR if the file already exists
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_create_directory(const char* path);

/**
 * @brief Lists the contents of a directory.
 *
 * @param path the directory path
 *
 * @return an array containing the names of each directory entry
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
Array(String) file_list_directory(const char* path);

#endif

/**
 * @brief A bitmask enum representing the file opening modes.
 */
typedef enum {
    FILE_READ         = 1 << 0,
    FILE_WRITE        = 1 << 1,
    FILE_APPEND       = 1 << 2,
    FILE_TRUNCATE     = 1 << 3
} FileOpenOption;

/**
 * @brief Opens a file.
 *
 * @param path the file path
 * @param modes the file opening modes
 *
 * @return the opened file, or nullptr if the operation fails
 *
 * @exception NULL_POINTER_ERROR if path or modes is null
 * @exception ILLEGAL_ARGUMENT_ERROR if the file open modes is unknown
 * @exception FILE_NOT_FOUND_ERROR if the file does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the file object fails
 */
File* file_open(const char* path, FileOpenOption modes);

/**
 * @brief Creates and opens a temporary file.
 *
 * @return the created temporary file, or nullptr if the operation fails
 *
 * @exception FILE_ACCESS_DENIED_ERROR if access to the temporary file is denied
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the file object fails
 */
File* file_open_temporary(void);

/**
 * @brief Closes a file.
 *
 * @param file the file to close
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_close(File* file);

/**
 * @brief Reads data from a file.
 *
 * @param file the file to read from
 * @param buffer the buffer to store the data
 * @param size the number of bytes to read
 *
 * @return the number of bytes read
 *
 * @exception NULL_POINTER_ERROR if file or buffer is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
bytes file_read(File* file, void* buffer, bytes size);

/**
 * @brief Reads a character from a file.
 *
 * @param file the file to read from
 *
 * @return the character read, or EOF if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
int file_read_char(File* file);

/**
 * @brief Reads a string line from a file.
 *
 * @param file the file to read from
 * @param ... optional buffer size
 *
 * @return the string containing the line, or nullptr if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the string fails
 */
#define file_read_line(file, ...) file_read_line_(file, __VA_OPT__(__VA_ARGS__,) 256, __VA_ARGS__)

#define file_read_line_(file, size, ...) file_read_line(file, (char[size]){}, size)

String (file_read_line)(File* file, char* buffer, bytes size);

/**
 * @brief Writes data to a file.
 *
 * @param file the file to write to
 * @param buffer the data to write
 * @param size the number of bytes to write
 *
 * @return the number of bytes written
 *
 * @exception NULL_POINTER_ERROR if file or buffer is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
bytes file_write(File* file, const void* buffer, bytes size);

/**
 * @brief Writes a character to a file.
 *
 * @param file the file to write to
 * @param character the character to write
 *
 * @return the character written, or EOF if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
int file_write_char(File* file, int character);

/**
 * @brief Writes a formatted string to a file.
 *
 * @param file the file to write to
 * @param string the string format
 * @param ... optional arguments
 *
 * @return the number of characters written, or a negative value if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file or string.data is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
#define file_write_string(file, string, ...) file_write_string(file, dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

int (file_write_string)(File* file, struct String string, ...);

/**
 * @brief Enumeration of file seek origins.
 */
typedef enum {
    FILE_SEEK_BEGIN,
    FILE_SEEK_CURRENT,
    FILE_SEEK_END
} FileSeekOrigin;

/**
 * @brief Changes the file position.
 *
 * @param file the file whose position to change
 * @param offset the offset from the origin
 * @param origin the position from which the offset is calculated
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_seek(File* file, long offset, FileSeekOrigin origin);

/**
 * @brief Gets the current file position.
 *
 * @param file the file whose position to get
 *
 * @return the current file position, or -1 if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
long file_position(File* file);

/**
 * @brief Resets the file position to the beginning.
 *
 * @param file the file to rewind
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_rewind(File* file);

/**
 * @brief Checks whether the end of the file has been reached.
 *
 * @param file the file to check
 *
 * @return true if the end of the file has been reached, false otherwise
 *
 * @exception NULL_POINTER_ERROR if file is null
 */
bool file_at_end(File* file);

/**
 * @brief Flushes the file.
 *
 * @param file the file to flush
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_flush(File* file);

#ifdef __linux__

/**
 * @brief Changes the size of a file.
 *
 * @param file the file to truncate
 * @param size the new file size in bytes
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_truncate(File* file, bytes size);

#endif

/**
 * @brief Checks whether a file exists.
 *
 * @param path the file path
 *
 * @return true if the file exists, false otherwise
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
bool file_exists(const char* path);

/**
 * @brief Retrieves the size of a file.
 *
 * @param file the file
 *
 * @return the size in bytes, or -1 if the operation fails
 *
 * @exception NULL_POINTER_ERROR if file is null
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
bytes file_size(File* file);

#ifdef __linux__

/**
 * @brief Represents a file metadata.
 */
typedef struct {
    bytes size;
    bool is_directory;
    bool is_regular;
    bool is_symbolic_link;
    int64 creation_time;
    int64 modified_time;
    int64 access_time;
} FileInfo;

/**
 * @brief Retrieves the metadata of a file.
 *
 * @param path the file path
 *
 * @return the file metadata
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_NOT_FOUND_ERROR if the file does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
FileInfo file_info(const char* path);

#endif

/**
 * @brief Copies a file to a destination path.
 *
 * @param source the path of the file to copy
 * @param destination the destination path
 *
 * @exception NULL_POINTER_ERROR if source or destination is null
 * @exception FILE_NOT_FOUND_ERROR if source does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to either files is denied
 * @exception FILE_INPUT_OUTPUT_ERROR if an I/O error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_copy(const char* source, const char* destination);

/**
 * @brief Moves or renames a file.
 *
 * @param old_path the old file path
 * @param new_path the new file path
 *
 * @exception NULL_POINTER_ERROR if old_path or new_path is null
 * @exception FILE_NOT_FOUND_ERROR if the source file does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_move(const char* old_path, const char* new_path);

/**
 * @brief Deletes a file.
 *
 * @param path the file path
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_NOT_FOUND_ERROR if the file does not exist
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_DIRECTORY_NOT_EMPTY_ERROR if the directory is not empty
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
void file_delete(const char* path);

/**
 * @brief Deletes a file if it exists.
 *
 * @param path the file path
 *
 * @return true if the file was successfully deleted, false if it does not exist
 *
 * @exception NULL_POINTER_ERROR if path is null
 * @exception FILE_ACCESS_DENIED_ERROR if access to the file is denied
 * @exception FILE_DIRECTORY_NOT_EMPTY_ERROR if the directory is not empty
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
bool file_delete_if_exists(const char* path);

#endif
