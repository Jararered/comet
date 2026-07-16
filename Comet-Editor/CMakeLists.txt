cmake_minimum_required(VERSION 3.20)

add_executable(Comet-Editor)

target_include_directories(Comet-Editor
PRIVATE
    source
    ${CMAKE_SOURCE_DIR}/libraries/glm/include
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

add_custom_command(TARGET Comet-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:Comet-Editor>/Resources"
    COMMENT "Copying Resources next to Comet-Editor executable"
)

install(DIRECTORY Resources
    DESTINATION ${CMAKE_SOURCE_DIR}/build/Comet-Editor
)
