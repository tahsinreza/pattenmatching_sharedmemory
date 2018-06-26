cmake_minimum_required (VERSION 3.10)
project(patternmatching_utils LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${SOURCE_ROOT_DIR}/totem)


add_library(${PROJECT_NAME}
        backward_compatibility.h
        bitmap_dynamic.cc
        bitmap_dynamic.h
        bitmap_dynamic.tpp
        bitmap_fixed.h
        bitmap_fixed.tpp
        edge.cc
        edge.h
        logger.cc
        logger.h
        cmd_line_option.cc
        cmd_line_option.h
        utils.h
        walk.cc
        walk.h)

target_link_libraries(${PROJECT_NAME}
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})


