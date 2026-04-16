cmake_minimum_required(VERSION 3.16)
project(ZenDemos C)

set(CMAKE_C_STANDARD 99)

# Find SDL2 and SDL2_mixer
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer QUIET)

# Common include
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})

# Math library (needed on some platforms)
if(UNIX)
    set(MATH_LIB m)
endif()

# SDL2_mixer - try pkg-config as fallback
if(NOT SDL2_mixer_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_MIXER SDL2_mixer)
    endif()
endif()

# Determine mixer link flags (optional - demos work without sound)
set(MIXER_LIBS "")
if(SDL2_mixer_FOUND)
    set(MIXER_LIBS SDL2_mixer::SDL2_mixer)
    message(STATUS "SDL2_mixer found - music enabled")
elseif(SDL2_MIXER_FOUND)
    set(MIXER_LIBS ${SDL2_MIXER_LIBRARIES})
    include_directories(${SDL2_MIXER_INCLUDE_DIRS})
    message(STATUS "SDL2_mixer found (pkg-config) - music enabled")
else()
    # Check if the library exists before trying to link
    find_library(SDL2_MIXER_LIB NAMES SDL2_mixer)
    if(SDL2_MIXER_LIB)
        set(MIXER_LIBS ${SDL2_MIXER_LIB})
        message(STATUS "SDL2_mixer found at ${SDL2_MIXER_LIB} - music enabled")
    else()
        message(STATUS "SDL2_mixer NOT found - building without music")
    endif()
endif()

# List of all demos
set(DEMOS
    01_flames
    02_fraktal
    03_chaos
    04_starfly
    05_shadebob
    06_tunnel
    07_pixelreb
    08_wobbler
    09_sinewave
    10_plasma
    11_voxel
    12_particles
    13_rotozoom
    14_w3d
    15_rat
    16_nnt
)

# Build each demo as a separate executable
foreach(DEMO ${DEMOS})
    add_executable(${DEMO} src/${DEMO}.c)
    target_link_libraries(${DEMO} ${SDL2_LIBRARIES} ${MIXER_LIBS} ${MATH_LIB})

    # On Windows with vcpkg or manual SDL2, may need SDL2main
    if(WIN32)
        target_link_libraries(${DEMO} SDL2::SDL2 SDL2::SDL2main)
    endif()
endforeach()

# Copy music/data files next to executables
file(GLOB DATA_FILES ${CMAKE_SOURCE_DIR}/data/*.XM ${CMAKE_SOURCE_DIR}/data/*.xm ${CMAKE_SOURCE_DIR}/data/*.MOD ${CMAKE_SOURCE_DIR}/data/*.mod)
file(COPY ${DATA_FILES} DESTINATION ${CMAKE_BINARY_DIR}/data)

# Copy NNT assets (PCX, BMP, XM)
if(EXISTS ${CMAKE_SOURCE_DIR}/data/nnt)
    file(COPY ${CMAKE_SOURCE_DIR}/data/nnt DESTINATION ${CMAKE_BINARY_DIR}/data)
endif()
