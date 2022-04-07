cmake_minimum_required(VERSION 3.21)
project(crd)

set(CMAKE_CXX_STANDARD 20)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory( 3rdparty )

file( GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/*/*.hpp )

add_library( crd INTERFACE )
add_library(crd::crd ALIAS crd)

target_sources( crd INTERFACE ${src_files} )

target_include_directories( crd INTERFACE "${PROJECT_SOURCE_DIR}" )
target_include_directories( crd INTERFACE "${PROJECT_SOURCE_DIR}/3rdparty" )
target_include_directories( crd INTERFACE "${PROJECT_SOURCE_DIR}/3rdparty/glad/include" )
target_link_libraries( crd INTERFACE glad glfw assimp )

add_executable( exampleStencil example/exampleStencil.cpp)
target_link_libraries( exampleStencil crd )

add_executable( SSAO "example/examples/SSAO/ssao.cpp")
target_link_libraries( SSAO crd stb_image )

