cmake_minimum_required(VERSION 3.10)
project(zestaw8 C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(GCC_COVERAGE_LINK_FLAGS    "-lm")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
find_package(Threads REQUIRED)
add_executable(zestaw8 main.c)
target_link_libraries(zestaw8 Threads::Threads)

