cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(pmt_analysis)

find_package(ROOT REQUIRED)
include(RootNewMacros.cmake)

add_subdirectory(data_structure)
add_subdirectory(algorithm)

#set(SCRIPTS
#  maketemplate
#  rootlogon.C
#  )

#foreach(_script ${SCRIPTS})
#  configure_file(
#    scripts/${_script}
#    ${CMAKE_INSTALL_PREFIX}/${_script}   
#    )
#endforeach()

#add_custom_target(basic DEPENDS kernel scripts)
