cmake_minimum_required(VERSION 3.13)
project(kumalloc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")

include_directories(src)

add_library(kumalloc src/kumalloc.c)

add_executable(test main.c)
target_link_libraries(test kumalloc)
