cmake_minimum_required(VERSION 3.20)

add_library(Comet STATIC)

if(UNIX)
    find_package(glfw3 REQUIRED)
endif()

target_compile_definitions(Comet
PRIVATE
    GLFW_INCLUDE_NONE
    GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(Comet
PUBLIC
    source
)

target_include_directories(Comet
PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/glad/include
    ${CMAKE_SOURCE_DIR}/libraries/glfw/include
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/imgui/include
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

target_link_libraries(Comet
PRIVATE
    glfw
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)
file(GLOB_RECURSE IMGUI_CPP_FILES ${CMAKE_SOURCE_DIR}/libraries/imgui/source/*.cpp)

target_sources(Comet
PRIVATE
    ${CPP_FILES}
    ${IMGUI_CPP_FILES}
    ${CMAKE_SOURCE_DIR}/libraries/glad/source/gl.c
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/source/stb_image.cpp
)