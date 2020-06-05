cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE all_SRCS *.cpp)

add_executable(hestia-Roguelike.app ${all_SRCS} ../include)
target_include_directories(hestia-Roguelike.app PRIVATE ../include)
target_link_libraries(hestia-Roguelike.app PUBLIC hestia roguelike_project_options)