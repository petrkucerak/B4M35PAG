cmake_minimum_required(VERSION 3.5)
project(LabMPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})

add_executable(HelloWorld ../src/HelloWorld.cpp)
target_compile_options(HelloWorld PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(HelloWorld ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(SendAndReceive ../src/SendAndReceive.cpp)
target_compile_options(SendAndReceive PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(SendAndReceive ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(VectorNormalization ../src/VectorNormalization.cpp ../src/Utils.hpp)
target_compile_options(VectorNormalization PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(VectorNormalization ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(BroadcastDemo ../src/demo_codes/BroadcastDemo.cpp)
target_compile_options(BroadcastDemo PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(BroadcastDemo ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(GatherDemo ../src/demo_codes/GatherDemo.cpp)
target_compile_options(GatherDemo PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(GatherDemo ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(ReduceDemo ../src/demo_codes/ReduceDemo.cpp)
target_compile_options(ReduceDemo PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(ReduceDemo ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

add_executable(ScatterDemo ../src/demo_codes/ScatterDemo.cpp)
target_compile_options(ScatterDemo PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(ScatterDemo ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

