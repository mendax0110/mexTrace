cmake_minimum_required(VERSION 3.12)
project(mexTrace VERSION 1.0.0 LANGUAGES CXX)

# Options
option(MEXTRACE_ENABLE_COLOR "Enable colored output" ON)
option(MEXTRACE_BUILD_TESTS "Build tests" OFF)

# Standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific settings
if(WIN32)
    add_definitions(-DMEXTRACE_WINDOWS)
elseif(APPLE)
    add_definitions(-DMEXTRACE_MACOS)
elseif(UNIX)
    add_definitions(-DMEXTRACE_LINUX)
endif()

# Library sources
set(SOURCES
        src/StackFrame.cpp
        src/StackTrace.cpp
        src/SymbolResolver.cpp
        src/ConsolePrinter.cpp
        src/PlatformUtils.cpp
)

# Headers
set(HEADERS
        include/StackFrame.h
        include/StackTrace.h
        include/SymbolResolver.h
        include/ConsolePrinter.h
        include/PlatformUtils.h
)

if(WIN32)
    list(APPEND SOURCES src/WindowsSymbolResolver.cpp)
    list(APPEND HEADERS include/WindowsSymbolResolver.h)
else()
    list(APPEND SOURCES src/UnixSymbolResolver.cpp)
    list(APPEND HEADERS include/UnixSymbolResolver.h)
endif()

# Create library
add_library(mexTrace STATIC ${SOURCES} ${HEADERS})
target_include_directories(mexTrace PUBLIC include)

# Executable
add_executable(mexTrace_cli src/mexTrace.cpp)
target_link_libraries(mexTrace_cli mexTrace)