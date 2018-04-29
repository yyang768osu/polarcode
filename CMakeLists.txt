cmake_minimum_required(VERSION 2.8.9)
project(polarcode)


include_directories(inc)
file(GLOB SOURCES "src/*.cpp")

add_executable(polarcode ${SOURCES})

target_compile_options(polarcode PUBLIC -g)