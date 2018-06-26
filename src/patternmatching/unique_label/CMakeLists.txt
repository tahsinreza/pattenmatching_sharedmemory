cmake_minimum_required (VERSION 3.10)
project(patternmatching_unique_label LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${SOURCE_ROOT_DIR}/totem)


add_library(${PROJECT_NAME}
        unique_label_cc_cpu.h
        unique_label_cc_cpu.tpp
        unique_label_circular_constraint.cc
        unique_label_circular_constraint.h
        unique_label_common_cpu.h
        unique_label_common_cpu.tpp
        unique_label_lcc_cpu.h
        unique_label_lcc_cpu.tpp)

target_link_libraries(${PROJECT_NAME}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})


