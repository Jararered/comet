cmake_minimum_required(VERSION 3.20)

add_library(Comet STATIC)

target_compile_definitions(Comet
PRIVATE
    GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(Comet
PUBLIC
    source
)

target_include_directories(Comet
PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/imgui/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

target_link_libraries(Comet
PUBLIC
    raylib
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)

set(IMGUI_CPP_FILES
    ${CMAKE_SOURCE_DIR}/libraries/imgui/source/imgui.cpp
    ${CMAKE_SOURCE_DIR}/libraries/imgui/source/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/libraries/imgui/source/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/libraries/imgui/source/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/libraries/imgui/source/imgui_widgets.cpp
)

target_sources(Comet
PRIVATE
    ${CPP_FILES}
    ${IMGUI_CPP_FILES}
)
