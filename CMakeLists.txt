cmake_minimum_required(VERSION 3.21)
project(crenderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(crenderer main.cpp)
target_link_libraries( crenderer ${OPENCL_LIBRARIES} )

