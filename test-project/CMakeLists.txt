cmake_minimum_required(VERSION 3.5)

option(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(test-project LANGUAGES CXX)

add_executable(test-project "main.cpp" "some.h" "some.cpp" "mymzyr.cpp")
