cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(macros)

include_directories(${data_structure_SOURCE_DIR})

FILE(GLOB macros ${PROJECT_SOURCE_DIR}/*.C)
set(SCRIPTS
  rootlogon.C
  )

foreach(_script ${SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${CMAKE_BINARY_DIR}/${_script}   
    )
  list(REMOVE_ITEM macros ${PROJECT_SOURCE_DIR}/${_script})
endforeach()


#foreach(_macro ${macros})
  #install(FILES ${_macro} DESTINATION ${CMAKE_BINARY_DIR})
  #file(COPY ${_macro} DESTINATION ${CMAKE_BINARY_DIR})
  #message(STATUS ${_macro})
#endforeach()
#--------------------------


add_custom_target(macros ALL cp ${macros} ${CMAKE_BINARY_DIR} DEPENDS ${macros} SOURCES ${macros})
