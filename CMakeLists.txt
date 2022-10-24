cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CMAKE_CXX_STANDARD 23)

# set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
project(${PROJECT_NAME})

if(${CMAKE_CL_64})
    set(CMAKE_64_BIT_POSTFIX -x64)
endif(${CMAKE_CL_64})

set(CMAKE_DEBUG_POSTFIX -debug)

include_directories(
    game
    source

    libraries/glad/include
    libraries/glfw/include
    libraries/glm/include
    libraries/imgui/include
    libraries/stb_image/include
    libraries/FastNoiseLite/include
    libraries/cereal/include
)

# Added so that glad would shut the fuck up
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

link_directories(
    libraries/glfw/lib
)

find_package(glfw3 3.3 PATHS libraries/glfw/lib)

# Game files
file(GLOB_RECURSE GAME_CPP_FILES game/*.cpp)
file(GLOB_RECURSE GAME_H_FILES game/*.h)

# Engine files
file(GLOB_RECURSE ENGINE_CPP_FILES source/*.cpp)
file(GLOB_RECURSE ENGINE_C_FILES source/*.c)
file(GLOB_RECURSE ENGINE_H_FILES source/*.h)
file(GLOB_RECURSE IMGUI_CPP_FILES libraries/imgui/source/*.cpp)

add_executable(
    ${PROJECT_NAME}

    ${GAME_CPP_FILES}
    ${GAME_H_FILES}

    ${ENGINE_CPP_FILES}
    ${ENGINE_C_FILES}
    ${ENGINE_H_FILES}

    ${IMGUI_CPP_FILES}

    libraries/glad/source/gl.c
    libraries/stb_image/source/stb_image.cpp
)

target_precompile_headers(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:comet.pch>")

target_link_libraries(${PROJECT_NAME} glfw)

# Added to that OpenGL context does not start terminal
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
