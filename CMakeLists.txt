cmake_minimum_required(VERSION 2.8.9)
project(polarcode)

add_compile_options(-g)

include_directories(inc)
file(GLOB SOURCES "src/*.cpp")

add_executable(polarcode ${SOURCES})
