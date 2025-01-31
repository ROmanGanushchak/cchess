cmake_minimum_required(VERSION 3.24)
project(cchess)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")

add_subdirectory(engine)
add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE engine engine_moves)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/engine/moves)