cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(data_structure)

find_package(ROOT REQUIRED)
#include(ROOTConfig.cmake)
include(RootNewMacros.cmake)

include_directories(${ROOT_INCLUDE_DIRS})
#add_definitions(-std=c++11)

FILE(GLOB headers ${PROJECT_SOURCE_DIR}/*.h)
list(REMOVE_ITEM headers ${PROJECT_SOURCE_DIR}/LinkDef.h)
FILE(GLOB sources ${PROJECT_SOURCE_DIR}/*.cxx)
#set(sources PTAnaConfigData.cxx)
ROOT_GENERATE_DICTIONARY(DataStructure_Dict ${headers} LINKDEF LinkDef.h)

ROOT_LINKER_LIBRARY(data_structure 
		    ${sources}
		    DataStructure_Dict.cxx
		    LIBRARIES Core Cint RIO Hist Gpad Tree Rint)
		    
#-------------------------------------------------------
#install(TARGETS pmtanalysis DESTINATION lib)
#foreach(_header ${headers})
#  install(FILES ${_header} DESTINATION include)
#endforeach()