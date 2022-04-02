include_directories(glad/include)

add_subdirectory(glfw-3.3.6)
add_subdirectory(assimp)
add_library( glad glad/src/glad.c )