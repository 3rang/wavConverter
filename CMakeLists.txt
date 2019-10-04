cmake_minimum_required(VERSION 2.8.11)

set(LIB_NAME "wavTypeconvert")

file(GLOB SRC  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${LIB_NAME} SHARED ${SRC})

install(
    TARGETS ${LIB_NAME} 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/output)                                       
