cmake_minimum_required(VERSION 3.20)

get_filename_component(LIBRARY_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${LIBRARY_NAME})

# Added to silence GLAD when building
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

link_directories(libraries/glfw/lib/windows)

include_directories(
    source
    ${CMAKE_SOURCE_DIR}/libraries/glad/include
    ${CMAKE_SOURCE_DIR}/libraries/glfw/include
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/imgui/include
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)
file(GLOB_RECURSE HPP_FILES source/*.hpp)
file(GLOB_RECURSE IMGUI_CPP_FILES ${CMAKE_SOURCE_DIR}/libraries/imgui/source/*.cpp)

add_library(${LIBRARY_NAME}
    ${CPP_FILES}
    ${HPP_FILES}

    ${IMGUI_CPP_FILES}
    ${CMAKE_SOURCE_DIR}/libraries/glad/source/gl.c
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/source/stb_image.cpp
)

target_link_libraries(${LIBRARY_NAME} glfw3)

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 23)

install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION ${INSTALLDIR}/lib)
# install(DIRECTORY source/Comet DESTINATION ${INSTALLDIR}/include FILES_MATCHING PATTERN "*.hpp")
