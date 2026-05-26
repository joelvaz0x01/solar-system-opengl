# GLFW configuration - prioritize Wayland over X11 on Linux
if(UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(WAYLAND wayland-client wayland-cursor wayland-egl)

  if(WAYLAND_FOUND)
    message(STATUS "Wayland detected - building GLFW with Wayland support")
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
  else()
    message(STATUS "Wayland not found - falling back to X11 for GLFW")
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
  endif()
endif()

# GLFW
add_subdirectory(glfw)

# GLAD
add_library(GLAD "glad/src/gl.c")
target_include_directories(GLAD PUBLIC "glad/include")

# FreeType
add_subdirectory(freetype)
