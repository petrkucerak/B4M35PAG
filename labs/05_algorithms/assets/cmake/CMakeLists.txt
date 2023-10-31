cmake_minimum_required(VERSION 3.5)
project(LabMatrixAlgorithm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})

add_executable(Matrix_Tasks ../src/OpenMP_template/MatrixMultiplication.cpp)
add_executable(LU_Decomposition ../src/OpenMP_template/LUDecomposition.cpp)

add_executable(Gauss_Elimination_Block ../src/MPI_template/GaussEliminationBlock.cpp)
target_compile_options(Gauss_Elimination_Block PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(Gauss_Elimination_Block ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(Gauss_Elimination_Cyclic ../src/MPI_template/GaussEliminationCyclic.cpp)
target_compile_options(Gauss_Elimination_Cyclic PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(Gauss_Elimination_Cyclic ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})


