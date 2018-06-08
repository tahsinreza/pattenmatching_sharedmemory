cmake_minimum_required(VERSION 3.10)
project(patternmatching LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/algorithms
        ${SOURCE_ROOT_DIR}/totem)

set(PROJECT_TPP_SOURCES
        algorithms/totem_multiple_label_cc_cpu.tpp
        algorithms/totem_multiple_label_common_cpu.tpp
        algorithms/totem_multiple_label_lcc_cpu.tpp
        algorithms/totem_multiple_label_pc_cpu.tpp
        algorithms/totem_unique_label_cc_cpu.tpp
        algorithms/totem_unique_label_common_cpu.tpp
        algorithms/totem_unique_label_lcc_cpu.tpp
        )

set(PROJECT_CPP_SOURCES
        totem_multiple_label_step.cc
        totem_patternmatching_logger.cc
        totem_patternmatching_cmd_line_option.cc
        totem_unique_label_step.cc)

set(PROJECT_CUDA_SOURCES
        totem_multiple_label_cpu.cu
        totem_patternmatching_binary.cu
        totem_unique_label_cpu.cu)

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
