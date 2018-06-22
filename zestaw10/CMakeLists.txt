cmake_minimum_required(VERSION 3.10)
project(zestaw10 C)

set(CMAKE_C_STANDARD 11)

add_executable(zestaw10 main.c zad1/spec.h zad1/client.c zad1/server.c zad2/spec.h zad2/server.c zad2/client.c)