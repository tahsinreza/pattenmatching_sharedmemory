cmake_minimum_required(VERSION 3.10)

include(CMakeConfig.cmake)

# set basic directory structure
set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_ROOT_DIR ${ROOT_DIR}/src)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(BUILD_ROOT_DIR ${ROOT_DIR}/build/${CMAKE_BUILD_TYPE}/)
else()
    set(BUILD_ROOT_DIR ${ROOT_DIR}/build)
endif()

# configure cmake output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/bin)

# add source directory
add_subdirectory(${SOURCE_ROOT_DIR})


