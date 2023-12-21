cmake_minimum_required(VERSION 3.5)
project(solar_system VERSION 1.0)

# set output directory to ${CMAKE_SOURCE_DIR}/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

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

file(GLOB SHADERS
		"src/shaders/*.glsl"
)

file(GLOB SRC_SOLAR_SYSTEM
		"src/*.h"
		"src/*.cpp"
		${SHADERS}
)

add_executable(${SOLAR_SYSTEM} ${SRC_SOLAR_SYSTEM})
target_link_libraries(${SOLAR_SYSTEM} ${ALL_LIBS})

# create directory for shaders on ${CMAKE_SOURCE_DIR}/bin
if(WIN32)
	add_custom_command(TARGET ${SOLAR_SYSTEM} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${SOLAR_SYSTEM}>/shaders)
elseif(UNIX AND NOT APPLE)
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/shaders)
elseif(APPLE)
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/shaders)
endif(WIN32)

# copy shaders to ${CMAKE_SOURCE_DIR}/bin/shaders directory
foreach(SHADER ${SHADERS})
	if(WIN32)
		add_custom_command(
				TARGET ${SOLAR_SYSTEM} PRE_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${SOLAR_SYSTEM}>/shaders
		)
	elseif(UNIX AND NOT APPLE)
		file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
	elseif(APPLE)
		get_filename_component(SHADER_NAME ${SHADER} NAME)
		makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${SHADER_NAME} ${SOLAR_SYSTEM})
	endif(WIN32)
endforeach(SHADER)

# copy resources to ${CMAKE_SOURCE_DIR}/bin/resources directory
if(WIN32)
	add_custom_command(
			TARGET ${SOLAR_SYSTEM} PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${SOLAR_SYSTEM}>/resources
	)
elseif(UNIX AND NOT APPLE)
	file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
elseif(APPLE)
	makeLink(${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR}/bin/resources ${SOLAR_SYSTEM})
endif(WIN32)
