cmake_minimum_required(VERSION 3.10)

project(patternmatching_test LANGUAGES CXX CUDA)


include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PATTERNMATCHING_SOURCE_DIR}/unique_label
        ${PATTERNMATCHING_SOURCE_DIR}/multiple_label
        ${PATTERNMATCHING_SOURCE_DIR}/algorithms
        ${PATTERNMATCHING_SOURCE_DIR}/utils
        ${SOURCE_ROOT_DIR}/totem)

add_executable(${PROJECT_NAME}_bitmap_fixed
        ${PATTERNMATCHING_SOURCE_DIR}/utils/bitmap_fixed.h
        ${PATTERNMATCHING_SOURCE_DIR}/utils/bitmap_fixed.tpp
        bitmap_fixed.cc)

target_link_libraries(${PROJECT_NAME}_bitmap_fixed
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})

add_executable(${PROJECT_NAME}_bitmap_dynamic
        ${PATTERNMATCHING_SOURCE_DIR}/utils/bitmap_dynamic.cc
        bitmap_dynamic.cc)

target_link_libraries(${PROJECT_NAME}_bitmap_dynamic
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})


add_executable(${PROJECT_NAME}_graph_stat_effectiveness
        graph_stat_effectiveness_cpu.cc
        graph_stat_effectiveness_cpu.h
        graph_stat_effectiveness.cc
        ${PATTERNMATCHING_CPP_SOURCES})

target_link_libraries(${PROJECT_NAME}_graph_stat_effectiveness
        patternmatching_algorithms
        patternmatching_unique_label
        patternmatching_multiple_label
        patternmatching_utils
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})


add_executable(${PROJECT_NAME}_graph_make_undirected
        graph_make_undirected.cc
        graph_make_undirected_cpu.cc
        graph_make_undirected_cpu.h
        ${PATTERNMATCHING_CPP_SOURCES})

target_link_libraries(${PROJECT_NAME}_graph_make_undirected
        patternmatching_algorithms
        patternmatching_unique_label
        patternmatching_multiple_label
        patternmatching_utils
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})