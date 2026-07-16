cmake_minimum_required(VERSION 3.20)

add_library(Comet STATIC)

target_compile_definitions(Comet
PRIVATE
    GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(Comet
PUBLIC
    source
    ${CMAKE_SOURCE_DIR}/libraries/raygui
)

target_include_directories(Comet
PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

target_link_libraries(Comet
PUBLIC
    raylib
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)

target_sources(Comet
PRIVATE
    ${CPP_FILES}
)
