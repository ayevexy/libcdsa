cmake_minimum_required(VERSION 4.0)
project(libcdsa C)

set(CMAKE_C_STANDARD 23)

# ---- Library under test ----
add_library(libcdsa STATIC
        src/list/array_list.c
        src/util/iterator.c
        src/util/memory.c
        src/util/functions.c
)
target_include_directories(libcdsa PUBLIC src)

# ---- Dependencies ----
include(FetchContent)

# Unity Test Framework
FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(unity)

add_library(unity_test_framework ${unity_SOURCE_DIR}/src/unity.c)
target_include_directories(unity_test_framework PUBLIC
        ${unity_SOURCE_DIR}/src
)

# FFF (Fake Function Framework)
FetchContent_Declare(
        fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_TAG v1.1
)
FetchContent_MakeAvailable(fff)

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

    target_link_libraries(${EXECUTABLE}
            PRIVATE
            libcdsa
            unity_test_framework
    )
    target_include_directories(${EXECUTABLE} PRIVATE
            ${fff_SOURCE_DIR}
            src
    )

    add_test(NAME ${EXECUTABLE} COMMAND ${EXECUTABLE})

endforeach()
