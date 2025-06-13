cmake_minimum_required(VERSION 3.15)
project(mexTrace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)
find_package(Threads REQUIRED)

# Configure compiler options
add_compile_options(
        -Wall
        -Wextra
        -Werror
        -pedantic
        -fPIC
)

# Include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
)

# Source files
set(SOURCES
        src/ConsolePrinter.cpp
        src/PlatformUtils.cpp
        src/StackFrame.cpp
        src/StackTrace.cpp
        src/UnixSymbolReceiver.cpp
        src/mexTrace.cpp
        include/ErrorHandling.h
)

# Create executable
add_executable(${PROJECT_NAME}  ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads
        dl
)

# Install target
#install(TARGETS ${PROJECT_NAME} DESTINATION bin)