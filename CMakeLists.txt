cmake_minimum_required(VERSION 2.8.9)
project(polarcode)

set(CPP OFF CACHE BOOL "when switched on, only cpp target is built, otherwise pybind11 target is built.")

set(SOURCES
src/channel.cpp
src/crc.cpp
src/polar_construction.cpp
src/polar_decoder.cpp
src/polar_encoder.cpp
src/polar_list_decoder.cpp
src/polar.cpp
src/utilities.cpp
)

if(CPP)
  add_executable(polarcode ${SOURCES})
  target_compile_options(polarcode PUBLIC -g)
else()
  add_subdirectory(pybind11)
  pybind11_add_module(polarcode src/main.cpp ${SOURCES})
endif(CPP)


target_include_directories(polarcode PUBLIC inc)

