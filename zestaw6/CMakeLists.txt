cmake_minimum_required(VERSION 3.10)
project(zestaw6 C)

set(CMAKE_C_STANDARD 99)

add_executable(zestaw6 zad1/server.c zad1/client.c zad2/server.c zad2/client.c zad1/systemV.h zad2/posix.h)