# //////////////////////////////////////////////////////////////////////////
# ///
# /// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
# ///
# //////////////////////////////////////////////////////////////////////////

cmake_minimum_required(VERSION 3.13)

project(. VERSION 1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g -O0")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_BUILD_TYPE Debug)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    add_executable(test_cidev test_cidev.c)
    set_target_properties(test_cidev  PROPERTIES COMPILE_DEFINITIONS "__DEBUG_DISK;__DEBUG_TIMER")
    target_link_libraries(test_cidev  rt)
else()
    message(">>> CANNOT BUILD disk_timer: Linux is required!")
endif()
