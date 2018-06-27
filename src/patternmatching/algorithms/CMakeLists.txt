cmake_minimum_required (VERSION 3.10)
project(patternmatching_algorithms LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/utils
        ${SOURCE_ROOT_DIR}/totem)


add_library(${PROJECT_NAME}
        algo_results.h
        algo_results.cc
        graph_stat.h
        graph_stat.cc
        graph_stat_cpu.h
        graph_stat_cpu.tpp)

target_link_libraries(${PROJECT_NAME}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})


