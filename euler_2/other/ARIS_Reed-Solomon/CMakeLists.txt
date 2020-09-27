cmake_minimum_required(VERSION 3.17)
project(ARIS_Reed_Solomon C)

set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME} main.c RS/RS.c RS/RS.h)
