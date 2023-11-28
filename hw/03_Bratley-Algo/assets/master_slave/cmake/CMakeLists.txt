cmake_minimum_required(VERSION 3.5)
project(LabMPI_MasterSlave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})

add_executable(MasterSlave ../src/MasterSlave.cpp)
target_compile_options(MasterSlave PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(MasterSlave ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

