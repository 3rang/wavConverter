cmake_minimum_required(VERSION 2.8.11)

set(LIB_NAME "wavTypeconvert")

file(GLOB SRC  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB TEST_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/testapp/*.c)
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/build )

add_executable("testApp" ${TEST_SRC}) 

add_library(${LIB_NAME} SHARED ${SRC})
install(
    TARGETS ${LIB_NAME} 
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/output)                                       


target_link_libraries("testApp" libwavTypeconvert.so)
