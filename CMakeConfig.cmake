cmake_minimum_required(VERSION 3.10)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# support c++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# add OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    message("Found OpenMP!")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})
else ()
    message(FATAL_ERROR "OpenMP not found")
endif ()

# add CUDA
enable_language(CUDA)
find_package(CUDA REQUIRED)
if (CUDA_FOUND)
    message("Found CUDA!")

    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_STANDARD_REQUIRED OFF)
    set(CMAKE_CUDA_EXTENSIONS OFF)

    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xptxas -dlcm=cg")
    #set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xptxas -dlcm=cg -dc")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler \"${OpenMP_CXX_FLAGS}\"")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -g")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DFEATURE_SM35 -gencode=arch=compute_35,code=\\\"sm_35,compute_35\\\"")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D__STDC_LIMIT_MACROS")
    include_directories(SYSTEM ${CUDA_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "CUDA not found")
endif ()

# add TBB
set(TBB_LIBRARIES tbb)

# add other libraries
set(OTHER_LIBRARIES m)


# set warning level
set(WARNING_FLAGS "-Wall -Wno-sign-compare -Wno-format -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler \"${WARNING_FLAGS}\"")



