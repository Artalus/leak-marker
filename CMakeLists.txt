cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

project(leak-marker LANGUAGES CXX)

add_subdirectory(leak-marker)
add_subdirectory(leak-provider)
