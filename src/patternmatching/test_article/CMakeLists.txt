cmake_minimum_required(VERSION 3.10)

project(patternmatching_test LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/multiple_label_test
        ${PATTERNMATCHING_SOURCE_DIR}/algorithms
        ${PATTERNMATCHING_SOURCE_DIR}/utils
        ${SOURCE_ROOT_DIR}/totem)

macro(defineOptimizationExecutable name options)

add_executable(${PROJECT_NAME}_${name}
        test_optimization_multiple_label.cc
        test_optimization_multiple_label_cpu.cc
        test_optimization_multiple_label_cpu.h
        test_optimization_multiple_label_step.cc
        test_optimization_multiple_label_step.h
        ${PATTERNMATCHING_CPP_SOURCES})

target_link_libraries(${PROJECT_NAME}_${name}
        patternmatching_algorithms
        patternmatching_unique_label
        patternmatching_multiple_label_test_${name}
        patternmatching_utils
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})

target_compile_definitions(${PROJECT_NAME}_${name} PRIVATE ${options} ${ARGN})

endmacro()

defineOptimizationExecutable(optimization_none "")
defineOptimizationExecutable(optimization_work_aggregation OPTIMIZATION_USE_WORK_AGGREGATION)
defineOptimizationExecutable(optimization_early_termination OPTIMIZATION_USE_EARLY_TERMINATION)
defineOptimizationExecutable(optimization_multiple_validation OPTIMIZATION_USE_MULTIPLE_VALIDATION)
defineOptimizationExecutable(optimization_all OPTIMIZATION_USE_WORK_AGGREGATION OPTIMIZATION_USE_EARLY_TERMINATION OPTIMIZATION_USE_MULTIPLE_VALIDATION)


macro(defineOrderExecutable name options)

    add_executable(${PROJECT_NAME}_${name}
            test_order_multiple_label.cc
            test_order_multiple_label_cpu.cc
            test_order_multiple_label_cpu.h
            test_order_multiple_label_step.cc
            test_order_multiple_label_step.h
            ${PATTERNMATCHING_CPP_SOURCES})

    target_link_libraries(${PROJECT_NAME}_${name}
            patternmatching_algorithms
            patternmatching_unique_label
            patternmatching_multiple_label_test_optimization_all
            patternmatching_utils
            totem
            ${OTHER_LIBRARIES}
            ${TBB_LIBRARIES}
            ${OpenMP_CXX_LIBRARIES}
            ${CUDA_LIBRARIES})

    target_compile_definitions(${PROJECT_NAME}_${name} PRIVATE ${options} ${ARGN} OPTIMIZATION_USE_WORK_AGGREGATION OPTIMIZATION_USE_EARLY_TERMINATION OPTIMIZATION_USE_MULTIPLE_VALIDATION)

endmacro()

defineOrderExecutable(order_intuitive "")
defineOrderExecutable(order_effectiveness ORDER_USE_DYNAMIC)
defineOrderExecutable(order_enumerate_intuitive USE_ENUMERATE)
defineOrderExecutable(order_enumerate_effectiveness ORDER_USE_DYNAMIC USE_ENUMERATE)
defineOrderExecutable(order_enumerate_effectiveness_early ORDER_USE_DYNAMIC USE_ENUMERATE ENUMERATE_START_EARLY ENUMERATE_START_EARLY_EFFECTIVENESS_THRESHOLD=0.01)
