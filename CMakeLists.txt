cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(pmt_analysis)

find_package(ROOT REQUIRED)
include(RootNewMacros.cmake)

add_subdirectory(data_structure)
add_subdirectory(algorithm)
add_subdirectory(macros)

