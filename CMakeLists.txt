# ---- Project ----

cmake_minimum_required(VERSION 4.0)

project(libcdsa
        VERSION 1.0.0
        DESCRIPTION "Data Structures and Algorithms Library in C"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- Plugins ----

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

# ---- Library ----

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(${PROJECT_NAME}
        src/list/array_list.c
        src/util/iterator.c
        src/util/memory.c
        src/util/functions.c
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

# ---- Development ----

target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ---- Installation ----

# Describe Package
write_basic_package_version_file(
        ${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
)

# Install Library
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install Headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h"
)

# Export Targets
install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install Configuration Files
install(FILES
        ${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# ---- Test Dependencies ----

# Unity Test Framework
FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(unity)

add_library(unity_test_framework OBJECT
        ${unity_SOURCE_DIR}/src/unity.c
)
target_include_directories(unity_test_framework PUBLIC
        ${unity_SOURCE_DIR}/src
)

# FFF (Fake Function Framework)
FetchContent_Declare(
        fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_TAG v1.1
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fff)

add_library(fake_function_framework INTERFACE)
target_include_directories(fake_function_framework INTERFACE
        ${fff_SOURCE_DIR}
)

# ---- Tests ----

enable_testing()

file(GLOB_RECURSE TEST_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.c"
)

foreach(TEST_FILE ${TEST_FILES})

    get_filename_component(TEST_DIR ${TEST_FILE} DIRECTORY)
    get_filename_component(TEST_DIR ${TEST_DIR} NAME)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    set(EXECUTABLE "${TEST_DIR}.${TEST_NAME}")
    add_executable(${EXECUTABLE} ${TEST_FILE})

    target_link_libraries(${EXECUTABLE} PRIVATE
            ${PROJECT_NAME}
            unity_test_framework
            fake_function_framework
    )

    add_test(NAME ${EXECUTABLE} COMMAND ${EXECUTABLE})

endforeach()
