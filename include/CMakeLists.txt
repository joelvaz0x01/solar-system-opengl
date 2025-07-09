# GLFW configuration for X11 and Wayland support
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Enable both X11 and Wayland support in GLFW
    set(GLFW_BUILD_X11 ON CACHE BOOL "Build support for X11")
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build support for Wayland")
    
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
    
    # Try to find X11 libraries (optional for X11 support)
    find_package(X11)
    
    # Try to find Wayland libraries (optional for Wayland support)
    pkg_check_modules(WAYLAND wayland-client wayland-cursor wayland-egl)
    
    # Base OpenGL libraries for Linux
    set(OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        -lGL -lrt -lfreetype
        ${CMAKE_DL_LIBS}
        ${GLFW_LIBRARIES}
    )
    
    # Add X11 libraries if available (for X11 support)
    if(X11_FOUND)
        message(STATUS "X11 found - enabling X11 support")
        set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${X11_LIBRARIES}
        )
        if(X11_Xrandr_FOUND)
            set(OPENGL_LIBRARY ${OPENGL_LIBRARY} ${X11_Xrandr_LIB})
        endif()
        if(X11_Xext_FOUND)
            set(OPENGL_LIBRARY ${OPENGL_LIBRARY} ${X11_Xext_LIB})
        endif()
    endif()
    
    # Add Wayland libraries if available (for Wayland support)
    if(WAYLAND_FOUND)
        message(STATUS "Wayland found - enabling Wayland support")
        set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${WAYLAND_LIBRARIES}
        )
    endif()
    
    # Display configuration summary
    if(X11_FOUND AND WAYLAND_FOUND)
        message(STATUS "Building with both X11 and Wayland support")
    elseif(X11_FOUND)
        message(STATUS "Building with X11 support only")
    elseif(WAYLAND_FOUND)
        message(STATUS "Building with Wayland support only")
    else()
        message(FATAL_ERROR "Neither X11 nor Wayland libraries found. Cannot compile on Linux without display server support.")
    endif()
    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
