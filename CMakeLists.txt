cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_COMPILER "gcc-11")
set(CMAKE_CXX_COMPILER "g++-11")
SET(CMAKE_CXX_FLAGS "-Og -std=c++20")

#TO-DO 
#Select either multi-header or single-header install option
#Generate and install cmake config files for find_package()
#Generate and install doxygen docs

project(aviana_signal VERSION 0.9.0 LANGUAGES CXX)
set(SIGNAL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/signal")

add_subdirectory(try)
