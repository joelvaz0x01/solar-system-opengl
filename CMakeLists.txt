cmake_minimum_required(VERSION 3.10)
project(solar_system VERSION 1.0)

# set OpenGL policy to prefer GLVND libraries
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# set output directory to ${CMAKE_SOURCE_DIR}/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)

# makeLink function for macOS builds
function(makeLink src dest target)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}
        COMMENT "Creating symbolic link from ${src} to ${dest}"
    )
endfunction()

set(SOLAR_SYSTEM solar_system.out)

add_subdirectory(include)

# find fontconfig on Linux systems
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
    message(STATUS "Found Fontconfig: ${FONTCONFIG_LIBRARIES}")
endif()

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
        freetype
)

# add fontconfig on Linux systems
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND ALL_LIBS ${FONTCONFIG_LIBRARIES})
endif()

file(GLOB SHADERS
        "src/shaders/*.glsl"
)

file(GLOB SRC_SOLAR_SYSTEM
        "src/*.h"
        "src/main.cpp"
        ${SHADERS}
)

add_executable(${SOLAR_SYSTEM} ${SRC_SOLAR_SYSTEM})
target_link_libraries(${SOLAR_SYSTEM} ${ALL_LIBS})

# copy shaders to ${CMAKE_SOURCE_DIR}/bin/shaders directory
# POST_BUILD is to override shaders directory
if (WIN32)
    add_custom_command(
            TARGET ${SOLAR_SYSTEM} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:${SOLAR_SYSTEM}>/shaders
    )
elseif (UNIX AND NOT APPLE)
    file(COPY ${CMAKE_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
elseif (APPLE)
    makeLink(${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_SOURCE_DIR}/bin/shaders ${SOLAR_SYSTEM})
endif (WIN32)

# copy resources to ${CMAKE_SOURCE_DIR}/bin/resources directory
# POST_BUILD is to override resources directory
if (WIN32)
    add_custom_command(
            TARGET ${SOLAR_SYSTEM} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${SOLAR_SYSTEM}>/resources
    )
elseif (UNIX AND NOT APPLE)
    file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
elseif (APPLE)
    makeLink(${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR}/bin/resources ${SOLAR_SYSTEM})
endif (WIN32)
