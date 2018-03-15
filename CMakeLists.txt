cmake_minimum_required(VERSION 3.5)

option(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(leak-marker LANGUAGES CXX)

add_subdirectory(leak-marker)
add_subdirectory(test-project)
