cmake_minimum_required(VERSION 3.20)

add_executable(Comet-Editor)

target_include_directories(Comet-Editor
PRIVATE
    source
    ${CMAKE_SOURCE_DIR}/libraries/glad/include
    ${CMAKE_SOURCE_DIR}/libraries/glfw/include
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/imgui/include
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

target_link_libraries(Comet-Editor
PRIVATE
    Comet
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)

target_sources(Comet-Editor
PRIVATE
    main.cpp
    ${CPP_FILES}
)

# install(TARGETS Comet-Editor 
#     RUNTIME DESTINATION ${INSTALLDIR}/Comet-Editor
# )

# Copy the Resources directory to the build directory
# This is necessary for the cmake debug build to work
install(DIRECTORY Resources 
    DESTINATION ${CMAKE_SOURCE_DIR}/build/Comet-Editor
)
