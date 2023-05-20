cmake_minimum_required(VERSION 3.20)

get_filename_component(APP_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Added to silence GLAD when building
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

include_directories(
    source/Sandbox 

    ${CMAKE_SOURCE_DIR}/Comet/source
    ${CMAKE_SOURCE_DIR}/libraries/glad/include
    ${CMAKE_SOURCE_DIR}/libraries/glfw/include
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
    ${CMAKE_SOURCE_DIR}/libraries/imgui/include
    ${CMAKE_SOURCE_DIR}/libraries/stb_image/include
    ${CMAKE_SOURCE_DIR}/libraries/FastNoiseLite/include
    ${CMAKE_SOURCE_DIR}/libraries/cereal/include
)

link_directories(
    ${INSTALLDIR}/lib
    ${CMAKE_SOURCE_DIR}/libraries/glfw/lib/windows
)

file(GLOB_RECURSE CPP_FILES source/*.cpp)

add_executable(${APP_NAME} Main.cpp ${CPP_FILES})

target_link_libraries(${APP_NAME} Comet glfw3)

set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/windows/${APP_NAME}")

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${INSTALLDIR}/${APP_NAME})
install(DIRECTORY Shaders DESTINATION ${INSTALLDIR}/${APP_NAME})
install(DIRECTORY Textures DESTINATION ${INSTALLDIR}/${APP_NAME})
