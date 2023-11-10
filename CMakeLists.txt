cmake_minimum_required(VERSION 3.5)
project(Final_Project VERSION 1.0)
set(PROJECT project.out)
set(IMPORT_LIBS glfw GLEW::GLEW)
set(HEADER_FILES
    "include/common/shader.hpp"
)

# Find and include GLFW
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# Find and include GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Include OpenGL headers
include_directories(${OPENGL_INCLUDE_DIRS})

# final project (src)
file(GLOB SRC_PROJECT
    "include/common/shader.cpp"

    "src/main.cpp"
)
add_executable(${PROJECT} ${HEADER_FILES} ${SRC_PROJECT})
target_link_libraries(${PROJECT} ${IMPORT_LIBS})
target_include_directories(${PROJECT} PRIVATE include)

add_custom_command(
    TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)
