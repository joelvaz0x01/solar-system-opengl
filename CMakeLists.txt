cmake_minimum_required(VERSION 3.20)
project(solar_system VERSION 1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# standardize output directories to project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# OpenGL Policy
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# find OpenGL
find_package(OpenGL REQUIRED)

# disable unwanted parts of dependencies
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)

add_subdirectory(include)

# set executable name and source files
set(EXE_NAME solar_system)

set(SRC_FILES
  src/constants.h
  src/input.cpp
  src/input.h
  src/main.cpp
  src/main.h
  src/planet_data.cpp
  src/planet_data.h
  src/rendering.cpp
  src/rendering.h
  src/stb_image_impl.cpp
  src/text_renderer.cpp
  src/text_renderer.h
  src/types.h
  src/utils.cpp
  src/utils.h
)

# platform specific configurations
set(PLATFORM_LIBS)

if(APPLE)
  set(MACOS_BUNDLE MACOSX_BUNDLE)
elseif(WIN32)
  set(WIN32_GUI WIN32)
  if(ENABLE_WIN32_RESOURCES)
    list(APPEND SRC_FILES packaging/windows_processed.rc)
  endif()
elseif(LINUX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
  list(APPEND PLATFORM_LIBS ${FONTCONFIG_LIBRARIES})
endif()

add_executable(${EXE_NAME} ${MACOS_BUNDLE} ${WIN32_GUI} ${SRC_FILES})

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE /W4 /permissive-)
else()
  target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# include directories
target_include_directories(${EXE_NAME} PRIVATE
  include/common
  include/freetype/include
  include/glad/include
  include/glfw/include
  include/glm
  src
)

# link libraries
target_link_libraries(${EXE_NAME} PRIVATE
  freetype
  GLAD
  glfw
  OpenGL::GL
  ${PLATFORM_LIBS}
)

# resource handling (post-build)
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders)

if(APPLE AND MACOS_BUNDLE)
  add_custom_command(TARGET ${EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RES_DIR} $<TARGET_FILE_DIR:${EXE_NAME}>/../Resources/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} $<TARGET_FILE_DIR:${EXE_NAME}>/../Resources/shaders
    COMMENT "Copying resources into macOS App Bundle"
  )
else()
  add_custom_command(TARGET ${EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RES_DIR} $<TARGET_FILE_DIR:${EXE_NAME}>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} $<TARGET_FILE_DIR:${EXE_NAME}>/shaders
    COMMENT "Copying assets to output directory"
  )
endif()
