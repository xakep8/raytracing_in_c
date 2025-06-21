cmake_minimum_required(VERSION 3.10)
project(raytracing C)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Try to find SDL2 using different methods
find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
    # Fallback: try pkg-config
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 sdl2)
    endif()
endif()

if(NOT SDL2_FOUND)
    # Manual fallback
    find_path(SDL2_INCLUDE_DIRS SDL.h
        HINTS
        /usr/include/SDL2
        /usr/local/include/SDL2
        /opt/local/include/SDL2
    )
    
    find_library(SDL2_LIBRARIES
        NAMES SDL2
        HINTS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
    )
    
    if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES)
        set(SDL2_FOUND TRUE)
    endif()
endif()

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found! Please install SDL2 development libraries.")
endif()

# Create executable
add_executable(raytracing raytracing.c)

# Link libraries
target_link_libraries(raytracing ${SDL2_LIBRARIES} m)
target_include_directories(raytracing PRIVATE ${SDL2_INCLUDE_DIRS})

# Add compiler flags
target_compile_options(raytracing PRIVATE -Wall -Wextra)