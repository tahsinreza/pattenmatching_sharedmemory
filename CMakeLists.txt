cmake_minimum_required(VERSION 3.10)

include(CMakeConfig.cmake)


# add EID option This can still be set within the make call
if(EID STREQUAL "64")
    add_definitions(-DFEATURE_64BIT_EDGE_ID)
    set(EID_DIRECTORY EID64)
else()
    add_definitions(-DFEATURE_32BIT_EDGE_ID)
    set(EID_DIRECTORY .)
endif()

# set basic directory structure
set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_ROOT_DIR ${ROOT_DIR}/src)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(BUILD_ROOT_DIR ${ROOT_DIR}/build/${CMAKE_BUILD_TYPE}/${EID_DIRECTORY}/)
else()
    set(BUILD_ROOT_DIR ${ROOT_DIR}/build/${EID_DIRECTORY}/)
endif()

# configure cmake output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_ROOT_DIR}/bin)



# add source directory
add_subdirectory(${SOURCE_ROOT_DIR})


