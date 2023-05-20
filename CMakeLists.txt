cmake_minimum_required(VERSION 3.20)

project("Comet Engine + Game")

set(INSTALLDIR ${CMAKE_CURRENT_SOURCE_DIR}/windows)
message("Installation directory is set to: ${INSTALLDIR}")

add_subdirectory(Comet)
add_subdirectory(Sandbox)