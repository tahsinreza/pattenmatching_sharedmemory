cmake_minimum_required(VERSION 3.10)
project(patternmatching LANGUAGES CXX CUDA)

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
        unique_label_step.cc)

set(PROJECT_CUDA_SOURCES
        multiple_label_cpu.cu
        patternmatching_binary.cu
        unique_label_cpu.cu)

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
