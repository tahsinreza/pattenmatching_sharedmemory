cmake_minimum_required(VERSION 3.10)
project(totem LANGUAGES CXX CUDA)

include_directories(${PROJECT_SOURCE_DIR})

set(PROJECT_CPP_SOURCES
        totem_grooves_internal.cc)

set(PROJECT_CUDA_SOURCES
        totem_bitmap.cu
        totem.cu
        totem_graph.cu
        totem_hash_table.cu
        totem_partition.cu
        totem_util.cu
        totem_components.cu
        totem_engine.cu
        totem_grooves.cu
        totem_mem.cu
        totem_radix_sort.cu)

add_library(${PROJECT_NAME} STATIC
        ${PROJECT_CPP_SOURCES}
        ${PROJECT_CUDA_SOURCES})

target_link_libraries(${PROJECT_NAME}
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})
