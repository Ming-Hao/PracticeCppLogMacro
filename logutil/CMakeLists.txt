cmake_minimum_required(VERSION 3.14)
project(logutil LANGUAGES CXX)

# Create a header-only interface target
add_library(logutil STATIC
    logutil.cpp
)

target_compile_features(logutil PUBLIC cxx_std_11)

# Expose current directory as public include path
target_include_directories(logutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Optional: log format control (enabled by default)
option(LOGUTIL_USE_STRUCTURED_LOGGING "Enable structured logging with file, line, and function info" ON)
option(LOGUTIL_INCLUDE_FILE "Include file information in log format" ON)
option(LOGUTIL_INCLUDE_LINE "Include line information in log format" ON)
option(LOGUTIL_INCLUDE_FUNC "Include function information in log format" ON)
option(LOGUTIL_SHORTEN_PATH "Enable file path shortening with ellipsis" ON)
option(LOGUTIL_PATH_DEPTH "Number of directory levels to keep in file path" 5)

set(LOGUTIL_VERBOSE_LEVEL 2 CACHE STRING "Verbosity level: 0=trace, 1=debug, 2=info")

option(LOGUTIL_ENABLE_WARN "Enable WARN logs" ON)
option(LOGUTIL_ENABLE_ERROR "Enable ERROR logs" ON )

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

if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 0)
    message(STATUS "Enable TRACE")
    target_compile_definitions(logutil PUBLIC LOGUTIL_LOG_LEVELS_TRACE)
endif()

if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 1)
    message(STATUS "Enable DEBUG")
    target_compile_definitions(logutil PUBLIC LOGUTIL_LOG_LEVELS_DEBUG)
endif()

if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 2)
    message(STATUS "Enable INFO")
    target_compile_definitions(logutil PUBLIC LOGUTIL_LOG_LEVELS_INFO)
endif()

if (LOGUTIL_ENABLE_WARN)
    target_compile_definitions(logutil PUBLIC LOGUTIL_LOG_LEVELS_WARN)
endif()

if (LOGUTIL_ENABLE_ERROR)
    target_compile_definitions(logutil PUBLIC LOGUTIL_LOG_LEVELS_ERROR)
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
message(STATUS "│")
message(STATUS "│   Verbose Level      : ${LOGUTIL_VERBOSE_LEVEL}")

set(enabled_levels "")

if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 0)
    list(APPEND enabled_levels TRACE)
endif()
if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 1)
    list(APPEND enabled_levels DEBUG)
endif()
if(LOGUTIL_VERBOSE_LEVEL LESS_EQUAL 2)
    list(APPEND enabled_levels INFO)
endif()
if(LOGUTIL_ENABLE_WARN)
    list(APPEND enabled_levels WARN)
endif()
if(LOGUTIL_ENABLE_ERROR)
    list(APPEND enabled_levels ERROR)
endif()

string(REPLACE ";" ", " enabled_levels_str "${enabled_levels}")
message(STATUS "│   Enabled Log Levels : ${enabled_levels_str}")
message(STATUS "└──────────────────────────────")
message(STATUS "")