cmake_minimum_required(VERSION 3.14)
project(logutil LANGUAGES CXX)

# Create a header-only interface target
add_library(logutil STATIC
    logutil.cpp
)

# Expose current directory as public include path
target_include_directories(logutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Optional: log format control (enabled by default)
option(LOGUTIL_USE_STRUCTURED_LOGGING "Enable structured logging with file, line, and function info" ON)
option(LOGUTIL_INCLUDE_FILE "Include file information in log format" ON)
option(LOGUTIL_INCLUDE_LINE "Include line information in log format" ON)
option(LOGUTIL_INCLUDE_FUNC "Include function information in log format" ON)
option(LOGUTIL_SHORTEN_PATH "Enable file path shortening with ellipsis" ON)
option(LOGUTIL_PATH_DEPTH "Number of directory levels to keep in file path" 5)

# Define USE_STRUCTURED_LOGGING if requested
if (LOGUTIL_USE_STRUCTURED_LOGGING)
    target_compile_definitions(logutil PUBLIC USE_STRUCTURED_LOGGING)
endif()

# Define log format options if requested
if (LOGUTIL_INCLUDE_FILE)
    target_compile_definitions(logutil PUBLIC LOGUTIL_INCLUDE_FILE)
endif()
if (LOGUTIL_INCLUDE_LINE)
    target_compile_definitions(logutil PUBLIC LOGUTIL_INCLUDE_LINE)
endif()
if (LOGUTIL_INCLUDE_FUNC)
    target_compile_definitions(logutil PUBLIC LOGUTIL_INCLUDE_FUNC)
endif()

if (LOGUTIL_SHORTEN_PATH)
    target_compile_definitions(logutil PUBLIC LOGUTIL_SHORTEN_PATH)
    target_compile_definitions(logutil PUBLIC LOGUTIL_PATH_DEPTH=${LOGUTIL_PATH_DEPTH})
endif()

message(STATUS "")
message(STATUS "┌──── LogUtil Configuration ────")
message(STATUS "│   Structured Logging : ${LOGUTIL_USE_STRUCTURED_LOGGING}")
message(STATUS "│   Include File Info  : ${LOGUTIL_INCLUDE_FILE}")
message(STATUS "│   Include Line Info  : ${LOGUTIL_INCLUDE_LINE}")
message(STATUS "│   Include Func Info  : ${LOGUTIL_INCLUDE_FUNC}")
message(STATUS "│   Shorten File Path  : ${LOGUTIL_SHORTEN_PATH}")
if (LOGUTIL_SHORTEN_PATH)
    message(STATUS "│     Path Depth       : ${LOGUTIL_PATH_DEPTH}")
endif()
message(STATUS "└──────────────────────────────")
message(STATUS "")