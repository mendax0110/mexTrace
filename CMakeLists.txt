cmake_minimum_required(VERSION 3.25)
project(mexTrace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_compile_options(
        -Wall
        -Wextra
        -Werror
        -pedantic
        -fPIC
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
        src/ConsolePrinter.cpp
        src/PlatformUtils.cpp
        src/StackFrame.cpp
        src/StackTrace.cpp
        src/mexTrace.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Threads::Threads
        stdc++exp
        dl
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)