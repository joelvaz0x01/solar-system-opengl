cmake_minimum_required(VERSION 3.5)
project(Final_Project VERSION 1.0)

find_package(OpenGL REQUIRED)

set(PROJECT project.out)

# Find and include GLFW
add_subdirectory (include)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/include/glew/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(IMPORT_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
        -DGLEW_NO_GLU
)

set(HEADER_FILES
        "include/common/shader.hpp"
)

file(GLOB SRC_PROJECT
    "include/common/shader.cpp"

    "src/main.cpp"
)
add_executable(${PROJECT} ${HEADER_FILES} ${SRC_PROJECT})
target_link_libraries(${PROJECT} ${IMPORT_LIBS})

add_custom_command(
    TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PROJECT}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)
