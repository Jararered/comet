cmake_minimum_required(VERSION 3.30)

project(Comet)

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the installation directory
set(INSTALLDIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
message("Installation directory is set to: ${INSTALLDIR}")

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_MakeAvailable(raylib)

add_subdirectory(Comet)
add_subdirectory(Comet-Editor)
