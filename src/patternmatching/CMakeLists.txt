cmake_minimum_required(VERSION 3.10)
project(patternmatching LANGUAGES CXX CUDA)

set(PATTERNMATCHING_SOURCE_DIR ${SOURCE_ROOT_DIR}/patternmatching/)

add_subdirectory(test)
add_subdirectory(unique_label)
add_subdirectory(multiple_label)
add_subdirectory(algorithms)
add_subdirectory(utils)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/unique_label
        ${PATTERNMATCHING_SOURCE_DIR}/multiple_label
        ${PATTERNMATCHING_SOURCE_DIR}/algorithms
        ${PATTERNMATCHING_SOURCE_DIR}/utils
        ${SOURCE_ROOT_DIR}/totem)

add_executable(${PROJECT_NAME}
        patternmatching_binary.cc
        multiple_label_cpu.cc
        multiple_label_step.cc
        unique_label_cpu.cc
        unique_label_step.cc)

target_link_libraries(${PROJECT_NAME}
        patternmatching_algorithms
        patternmatching_unique_label
        patternmatching_multiple_label
        patternmatching_utils
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})

