cmake_minimum_required(VERSION 3.21)
project(crenderer)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCL REQUIRED )

add_executable( crenderer main.cpp)
add_executable( gpuTest gpu/test.cpp )

target_link_libraries( crenderer OpenCL::OpenCL )
target_link_libraries( gpuTest OpenCL::OpenCL )

