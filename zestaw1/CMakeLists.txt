cmake_minimum_required(VERSION 3.9)
project(zestaw1 C)

set(CMAKE_C_STANDARD 99)

add_executable(zestaw1 StaticArray.c StaticArray.h DynamicArray.h DynamicArray.c zad3a/test.c)