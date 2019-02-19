cmake_minimum_required(VERSION 3.10)
project(assignment_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(assignment_3 main.c smalllib.h list.c list.h smalllib.c signal_set.c signal_set.h)