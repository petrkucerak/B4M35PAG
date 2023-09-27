cmake_minimum_required(VERSION 3.5)
project(LabHelloWorld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(HelloWorldOpenMP ../src/HelloWorldOpenMP.cpp)

#MPI
find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})

add_executable(HelloWorldMPI ../src/HelloWorldMPI.cpp)
target_compile_options(HelloWorldMPI PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(HelloWorldMPI ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
