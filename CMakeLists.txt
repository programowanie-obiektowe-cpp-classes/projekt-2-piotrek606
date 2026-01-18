cmake_minimum_required(VERSION 3.20)
project(snake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

add_executable(snake
    src/main.cpp
    src/Config.cpp
    src/Random.cpp
    src/Board.cpp
    src/Snake.cpp
    src/Food.cpp
    src/Game.cpp
)

target_include_directories(snake PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(snake PRIVATE SFML::Graphics)

if(MSVC)
    target_compile_options(snake PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(snake PRIVATE -Wall -Wextra -pedantic)
endif()

add_custom_command(TARGET snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:snake>/data"
)

