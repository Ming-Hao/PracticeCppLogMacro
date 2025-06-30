cmake_minimum_required(VERSION 3.14)
project(myapp)

set(LOGUTIL_USE_STRUCTURED_LOGGING ON CACHE BOOL "Set from outer project")
set(LOGUTIL_INCLUDE_FUNC ON)

add_subdirectory(../logutil logutil_build)

add_executable(myapp 
    main.cpp
    foo.hpp
    foo.cpp
    TmpFolder/bar.hpp
    TmpFolder/bar.cpp)
    
target_link_libraries(myapp logutil)

# Customize log level prefixes
if(0)
target_compile_definitions(logutil PRIVATE    
    LOG_PREFIX_TRACE=T
    LOG_PREFIX_DEBUG=D
    LOG_PREFIX_INFO=I
    LOG_PREFIX_WARN=W
    LOG_PREFIX_ERROR=E
)
endif()
