cmake_minimum_required(VERSION 3.13)
project(data_mining_close C)

set(CMAKE_C_STANDARD 11)

add_executable(data_mining_close main.c binary_conversion.c binary_conversion.h close.c close.h)