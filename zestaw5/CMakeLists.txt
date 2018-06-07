cmake_minimum_required(VERSION 3.10)
project(zestaw5 C)

set(CMAKE_C_STANDARD 11)

add_executable(zestaw5 zad1/main.c zad2/master.c zad2/slave.c)