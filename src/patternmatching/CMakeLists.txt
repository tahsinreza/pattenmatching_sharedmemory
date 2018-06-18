cmake_minimum_required(VERSION 3.10)
project(patternmatching LANGUAGES CXX CUDA)

add_subdirectory(test)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/algorithms
        ${SOURCE_ROOT_DIR}/totem)

set(PROJECT_TPP_SOURCES
        algorithms/multiple_label_cc_cpu.tpp
        algorithms/multiple_label_common_cpu.tpp
        algorithms/multiple_label_lcc_cpu.tpp
        algorithms/multiple_label_pc_cpu.tpp
        algorithms/unique_label_cc_cpu.tpp
        algorithms/unique_label_common_cpu.tpp
        algorithms/unique_label_lcc_cpu.tpp
        )

set(PROJECT_CPP_SOURCES
        multiple_label_step.cc
        patternmatching_logger.cc
        patternmatching_cmd_line_option.cc
        unique_label_step.cc
        multiple_label_cpu.cc
        patternmatching_binary.cc
        unique_label_cpu.cc
        algorithms/bitmap.h
        algorithms/bitmap.tpp
        algorithms/unique_label_circular_constraint.cc
        algorithms/multiple_label_path_constraint.cc
        algorithms/multiple_label_path_constraint.h
        algorithms/multiple_label_local_constraint.cc
        algorithms/multiple_label_local_constraint.h
        algorithms/multiple_label_template_constraint.cc
        algorithms/multiple_label_template_constraint.h
        algorithms/multiple_label_common.h algorithms/multiple_label_template_walk.cc
        algorithms/multiple_label_common.h algorithms/multiple_label_template_edge.cc
        algorithms/multiple_label_common.h algorithms/multiple_label_circular_constraint.cc algorithms/multiple_label_circular_constraint.h)

set(PROJECT_CUDA_SOURCES)

add_executable(${PROJECT_NAME}
        ${PROJECT_TPP_SOURCES}
        ${PROJECT_CPP_SOURCES}
        ${PROJECT_CUDA_SOURCES})

target_link_libraries(${PROJECT_NAME}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})
