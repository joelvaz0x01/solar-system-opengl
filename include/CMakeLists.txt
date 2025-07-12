# GLFW configuration - prioritize Wayland over X11
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # First, check for Wayland availability
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND wayland-client wayland-cursor wayland-egl)
    
    if(WAYLAND_FOUND)
        # Wayland is available - build with Wayland-only support
        message(STATUS "Wayland detected - building with Wayland-only support")
        set(GLFW_BUILD_X11 OFF CACHE BOOL "Build support for X11")
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build support for Wayland")
        set(USE_WAYLAND TRUE)
    else()
        # Wayland not available - check for X11 as fallback
        find_package(X11 QUIET)
        
        # Check if X11 is fully available with required extensions (RandR, Xext)
        set(X11_COMPLETE FALSE)
        if(X11_FOUND AND X11_Xrandr_FOUND AND X11_Xext_FOUND)
            set(X11_COMPLETE TRUE)
            message(STATUS "Wayland not found - falling back to X11 support")
            set(GLFW_BUILD_X11 ON CACHE BOOL "Build support for X11")
            set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build support for Wayland")
            set(USE_WAYLAND FALSE)
        else()
            message(FATAL_ERROR "Neither Wayland nor complete X11 libraries found. Cannot compile on Linux without display server support.\n"
                               "Install Wayland libraries: sudo apt-get install libwayland-dev libwayland-cursor0 libwayland-egl1-mesa\n"
                               "Or install complete X11 libraries: sudo apt-get install libx11-dev libxrandr-dev libxext-dev")
        endif()
    endif()
    
    # Disable GLFW examples and tests to speed up build
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
endif()

# GLFW
add_subdirectory(glfw)
include_directories(glfw/include/GLFW)

# GLAD
add_library(GLAD "glad/src/gl.c")
target_include_directories(GLAD PRIVATE "glad/include")

# FreeType
add_subdirectory(freetype)
include_directories(freetype/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Find required packages for Linux
    find_package(PkgConfig REQUIRED)
    
    # Base OpenGL libraries for Linux
    set(OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        -lGL -lrt -lfreetype
        ${CMAKE_DL_LIBS}
        ${GLFW_LIBRARIES}
    )
    
    # Add libraries based on the display server we're using
    if(USE_WAYLAND)
        message(STATUS "Linking Wayland libraries")
        pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-cursor wayland-egl)
        set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${WAYLAND_LIBRARIES}
        )
    else()
        # Using X11 as fallback
        message(STATUS "Linking X11 libraries")
        find_package(X11 REQUIRED)
        set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${X11_LIBRARIES}
            ${X11_Xrandr_LIB}
            ${X11_Xext_LIB}
        )
    endif()
    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
