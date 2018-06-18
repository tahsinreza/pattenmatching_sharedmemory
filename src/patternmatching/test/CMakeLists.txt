cmake_minimum_required(VERSION 3.10)
project(patternmatching_test LANGUAGES CXX CUDA)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/..
        ${PROJECT_SOURCE_DIR}/../algorithms
        ${SOURCE_ROOT_DIR}/totem)

add_executable(${PROJECT_NAME}_bitmap
        bitmap.cc)

target_link_libraries(${PROJECT_NAME}_bitmap
        totem
        ${OTHER_LIBRARIES}
        ${TBB_LIBRARIES}
        ${OpenMP_CXX_LIBRARIES}
        ${CUDA_LIBRARIES})
