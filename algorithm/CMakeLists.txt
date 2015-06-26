cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(algorithm)

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${data_structure_SOURCE_DIR})

FILE(GLOB headers ${PROJECT_SOURCE_DIR}/*.h)
list(REMOVE_ITEM headers ${PROJECT_SOURCE_DIR}/LinkDef.h)
FILE(GLOB sources ${PROJECT_SOURCE_DIR}/*.cxx)
#set(sources)
ROOT_GENERATE_DICTIONARY(Algorithm_Dict ${headers} LINKDEF LinkDef.h)

ROOT_LINKER_LIBRARY(algorithm 
		    PTAnaBase.h
		    ${sources}
		    Algorithm_Dict.cxx
		    LIBRARIES Core Cint RIO Hist Gpad Tree Rint
		    DEPENDENCIES data_structure)
		    
#-------------------------------------------------------
#install(TARGETS pmtanalysis DESTINATION lib)
#foreach(_header ${headers})
#  install(FILES ${_header} DESTINATION include)
#endforeach()