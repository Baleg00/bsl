cmake_minimum_required(VERSION 3.0.0)
project(bsl VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories("./inc")

add_subdirectory("./src")
