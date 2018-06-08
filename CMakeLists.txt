cmake_minimum_required(VERSION 3.10)

include(CMakeConfig.cmake)

# set basic directory structure
set(ROOT_DIR ${CMAKE_BINARY_DIR}/..)
set(SOURCE_ROOT_DIR ${ROOT_DIR}/src)
set(BUILD_ROOT_DIR ${ROOT_DIR}/build)

# configure cmake output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/bin)


# add source directory
add_subdirectory(${SOURCE_ROOT_DIR})


