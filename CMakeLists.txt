cmake_minimum_required(VERSION 3.5)
project(solar_system VERSION 1.0)

find_package(OpenGL REQUIRED)

set(SOLAR_SYSTEM solar_system.out)

add_subdirectory (include)

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include/glfw/include
		${CMAKE_CURRENT_SOURCE_DIR}/include/glad/include
		${CMAKE_CURRENT_SOURCE_DIR}/include/common
		${CMAKE_CURRENT_SOURCE_DIR}/include/glm
)

add_library(GLAD "include/glad/src/glad.c")

set(ALL_LIBS
		${OPENGL_LIBRARY}
		glfw
		GLAD
)

file(GLOB SRC_SOLAR_SYSTEM
		"src/main.cpp"
)
add_executable(${SOLAR_SYSTEM} ${SRC_SOLAR_SYSTEM})
target_link_libraries(${SOLAR_SYSTEM} ${ALL_LIBS})

add_custom_command(
		TARGET ${SOLAR_SYSTEM} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${SOLAR_SYSTEM}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/bin/"
)
