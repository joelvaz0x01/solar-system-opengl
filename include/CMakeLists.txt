# GLFW configuration - detect X11 availability and configure accordingly
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # First, detect if X11 and required extensions are available
    find_package(X11 QUIET)
    
    # Check if X11 is fully available with required extensions (RandR, Xext)
    set(X11_COMPLETE FALSE)
    if(X11_FOUND AND X11_Xrandr_FOUND AND X11_Xext_FOUND)
        set(X11_COMPLETE TRUE)
        message(STATUS "X11 with RandR and Xext detected - building with X11 support")
    elseif(X11_FOUND)
        message(STATUS "X11 found but missing RandR or Xext extensions - falling back to Wayland-only")
    else()
        message(STATUS "X11 not detected - building with Wayland-only support")
    endif()
    
    if(X11_COMPLETE)
        # X11 is fully available - build with X11 support (and optionally Wayland)
        set(GLFW_BUILD_X11 ON CACHE BOOL "Build support for X11")
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build support for Wayland")
    else()
        # X11 not complete or not available - build Wayland-only
        set(GLFW_BUILD_X11 OFF CACHE BOOL "Build support for X11")
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build support for Wayland")
    endif()
    
    # Disable GLFW examples and tests to speed up build
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
endif()

# GLFW
add_subdirectory(glfw)
include_directories(glfw/include/GLFW)

# FreeType
add_subdirectory(freetype)
include_directories(freetype/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Find required packages for Linux
    find_package(PkgConfig REQUIRED)
    
    # Re-check X11 availability for library linking (using same logic as above)
    find_package(X11 QUIET)
    set(X11_COMPLETE FALSE)
    if(X11_FOUND AND X11_Xrandr_FOUND AND X11_Xext_FOUND)
        set(X11_COMPLETE TRUE)
    endif()
    
    # Try to find Wayland libraries
    pkg_check_modules(WAYLAND wayland-client wayland-cursor wayland-egl)
    
    # Base OpenGL libraries for Linux
    set(OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        -lGL -lrt -lfreetype
        ${CMAKE_DL_LIBS}
        ${GLFW_LIBRARIES}
    )
    
    # Add X11 libraries if complete X11 support is available
    if(X11_COMPLETE)
        message(STATUS "X11 with extensions found - linking X11 libraries")
        set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${X11_LIBRARIES}
            ${X11_Xrandr_LIB}
            ${X11_Xext_LIB}
        )
        
        # Also add Wayland if available for hybrid support
        if(WAYLAND_FOUND)
            message(STATUS "Wayland also found - enabling hybrid X11/Wayland support")
            set(OPENGL_LIBRARY
                ${OPENGL_LIBRARY}
                ${WAYLAND_LIBRARIES}
            )
        else()
            message(STATUS "Building with X11-only support")
        endif()
    else()
        # X11 not complete, ensure Wayland is available
        if(WAYLAND_FOUND)
            message(STATUS "Building with Wayland-only support")
            set(OPENGL_LIBRARY
                ${OPENGL_LIBRARY}
                ${WAYLAND_LIBRARIES}
            )
        else()
            message(FATAL_ERROR "Neither complete X11 nor Wayland libraries found. Cannot compile on Linux without display server support.\n"
                               "Install complete X11 libraries: sudo apt-get install libx11-dev libxrandr-dev libxext-dev\n"
                               "Or install Wayland libraries: sudo apt-get install libwayland-dev libwayland-cursor0 libwayland-egl1-mesa")
        endif()
    endif()
    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
