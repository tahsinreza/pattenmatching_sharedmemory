cmake_minimum_required(VERSION 3.10)
project(patternmatching LANGUAGES CXX CUDA)


set(PATTERNMATCHING_SOURCE_DIR ${SOURCE_ROOT_DIR}/patternmatching/)
set(PATTERNMATCHING_INCLUDE_DIR
        ${PATTERNMATCHING_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/unique_label
        ${PATTERNMATCHING_SOURCE_DIR}/multiple_label
        ${PATTERNMATCHING_SOURCE_DIR}/algorithms
        ${PATTERNMATCHING_SOURCE_DIR}/utils)

add_subdirectory(test)
add_subdirectory(unique_label)
add_subdirectory(multiple_label)
add_subdirectory(algorithms)
add_subdirectory(utils)

include_directories(
        ${PATTERNMATCHING_INCLUDE_DIR}
        ${SOURCE_ROOT_DIR}/totem)

set(PROJECT_CPP_SOURCES
        patternmatching_binary.cc
        patternmatching_logger.cc
        patternmatching_cmd_line_option.cc
        multiple_label_cpu.cc
        multiple_label_step.cc
        unique_label_cpu.cc
        unique_label_step.cc
        utils/backward_compatibility.h
        utils/bitmap_dynamic.h
        utils/bitmap_dynamic.cc
        utils/bitmap_dynamic.tpp
        utils/bitmap_fixed.h
        utils/bitmap_fixed.tpp
        utils/utils.h
        algorithms/graph_stat.cc
        algorithms/graph_stat_cpu.h
        algorithms/graph_stat_cpu.tpp
        algorithms/graph_stat.h
        multiple_label/multiple_label_cc_backtrack_cpu.h
        multiple_label/multiple_label_cc_backtrack_cpu.tpp
        multiple_label/multiple_label_cc_cpu.h
        multiple_label/multiple_label_cc_cpu.tpp
        multiple_label/multiple_label_cc_strict_cpu.h
        multiple_label/multiple_label_cc_strict_cpu.tpp
        multiple_label/multiple_label_constraint_circular.cc
        multiple_label/multiple_label_constraint_circular.h
        multiple_label/multiple_label_constraint_local.cc
        multiple_label/multiple_label_constraint_local.h
        multiple_label/multiple_label_constraint_path.cc
        multiple_label/multiple_label_constraint_path.h
        multiple_label/multiple_label_constraint_template.cc
        multiple_label/multiple_label_constraint_template.h
        multiple_label/multiple_label_generate_constraint_circular.cc
        multiple_label/multiple_label_generate_constraint_circular.h
        multiple_label/multiple_label_generate_constraint_local.cc
        multiple_label/multiple_label_generate_constraint_local.h
        multiple_label/multiple_label_generate_constraint_path.cc
        multiple_label/multiple_label_generate_constraint_path.h
        multiple_label/multiple_label_generate_constraint_template.cc
        multiple_label/multiple_label_generate_constraint_template.h
        multiple_label/multiple_label_generate_constraint_enumeration.cc
        multiple_label/multiple_label_generate_constraint_enumeration.h
        multiple_label/multiple_label_common_cpu.h
        multiple_label/multiple_label_common_cpu.tpp
        multiple_label/multiple_label_common.h
        multiple_label/multiple_label_lcc0_cpu.h
        multiple_label/multiple_label_lcc0_cpu.tpp
        multiple_label/multiple_label_lcc_cpu.h
        multiple_label/multiple_label_lcc_cpu.tpp
        multiple_label/multiple_label_pc_backtrack_cpu.h
        multiple_label/multiple_label_pc_backtrack_cpu.tpp
        multiple_label/multiple_label_pc_cpu.h
        multiple_label/multiple_label_pc_cpu.tpp
        multiple_label/multiple_label_pc_strict_cpu.h
        multiple_label/multiple_label_pc_strict_cpu.tpp
        multiple_label/multiple_label_tds_backtrack_cpu.h
        multiple_label/multiple_label_tds_backtrack_cpu.tpp
        multiple_label/multiple_label_tds_cpu.h
        multiple_label/multiple_label_tds_cpu.tpp
        multiple_label/multiple_label_tds_strict_cpu.h
        multiple_label/multiple_label_tds_strict_cpu.tpp
        multiple_label/multiple_label_enumeration_cpu.h
        multiple_label/multiple_label_enumeration_cpu.tpp
        utils/edge.cc
        utils/edge.h
        utils/walk.cc
        utils/walk.h
        unique_label/unique_label_cc_cpu.h
        unique_label/unique_label_cc_cpu.tpp
        unique_label/unique_label_circular_constraint.cc
        unique_label/unique_label_circular_constraint.h
        unique_label/unique_label_common_cpu.h
        unique_label/unique_label_common_cpu.tpp
        unique_label/unique_label_lcc_cpu.h
        unique_label/unique_label_lcc_cpu.tpp
        )

set(PROJECT_CUDA_SOURCES )

add_executable(${PROJECT_NAME}
        ${PROJECT_CPP_SOURCES}
        ${PROJECT_CUDA_SOURCES})

target_link_libraries(${PROJECT_NAME}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})

