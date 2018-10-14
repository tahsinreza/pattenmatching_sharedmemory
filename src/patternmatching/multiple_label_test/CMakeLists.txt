cmake_minimum_required (VERSION 3.10)
project(patternmatching_multiple_label_test LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/utils
        ${PATTERNMATCHING_SOURCE_DIR}/algorithms
        ${SOURCE_ROOT_DIR}/totem)



macro(defineOptimizationLibrary name options)

add_library(${PROJECT_NAME}_${name}
        multiple_label_cc_backtrack_cpu.h
        multiple_label_cc_backtrack_cpu.tpp
        multiple_label_cc_strict_cpu.h
        multiple_label_cc_strict_cpu.tpp
        multiple_label_common_cpu.h
        multiple_label_common_cpu.tpp
        multiple_label_common.h
        multiple_label_constraint_circular.cc
        multiple_label_constraint_circular.h
        multiple_label_constraint_local.cc
        multiple_label_constraint_local.h
        multiple_label_constraint_path.cc
        multiple_label_constraint_path.h
        multiple_label_constraint_template.cc
        multiple_label_constraint_template.h
        multiple_label_enumeration_cpu.h
        multiple_label_enumeration_cpu.tpp
        multiple_label_constraint_effectiveness.h
        multiple_label_constraint_effectiveness.cc
        multiple_label_constraint_effectiveness.tpp
        multiple_label_generate_constraint_circular.cc
        multiple_label_generate_constraint_circular.h
        multiple_label_generate_constraint_enumeration.cc
        multiple_label_generate_constraint_enumeration.h
        multiple_label_generate_constraint_local.cc
        multiple_label_generate_constraint_local.h
        multiple_label_generate_constraint_path.cc
        multiple_label_generate_constraint_path.h
        multiple_label_generate_constraint_template.cc
        multiple_label_generate_constraint_template.h
        multiple_label_lcc0_cpu.h
        multiple_label_lcc0_cpu.tpp
        multiple_label_lcc_cpu.h
        multiple_label_lcc_cpu.tpp
        multiple_label_pc_backtrack_cpu.h
        multiple_label_pc_backtrack_cpu.tpp
        multiple_label_pc_strict_cpu.h
        multiple_label_pc_strict_cpu.tpp
        multiple_label_tds_backtrack_cpu.h
        multiple_label_tds_backtrack_cpu.tpp
        multiple_label_tds_strict_cpu.h
        multiple_label_tds_strict_cpu.tpp)

target_link_libraries(${PROJECT_NAME}_${name}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})

target_compile_definitions(${PROJECT_NAME}_${name} PRIVATE ${options} ${ARGN})

endmacro()

defineOptimizationLibrary(optimization_none "")
defineOptimizationLibrary(optimization_work_aggregation OPTIMIZATION_USE_WORK_AGGREGATION)
defineOptimizationLibrary(optimization_early_termination OPTIMIZATION_USE_EARLY_TERMINATION)
defineOptimizationLibrary(optimization_multiple_validation OPTIMIZATION_USE_MULTIPLE_VALIDATION)
defineOptimizationLibrary(optimization_all OPTIMIZATION_USE_WORK_AGGREGATION OPTIMIZATION_USE_EARLY_TERMINATION OPTIMIZATION_USE_MULTIPLE_VALIDATION)

