cmake_minimum_required(VERSION 3.21)
project(crenderer)

set(CMAKE_CXX_STANDARD 20)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package( OpenCL REQUIRED )

include_directories(src)

add_subdirectory(3rdparty)

include_directories( 3rdparty/glad/include )
add_executable( testVecMat testVecMat.cpp )
target_link_libraries( testVecMat glad glfw assimp::assimp )
