cmake_minimum_required(VERSION 3.28.0)
project(Minor VERSION 1.0.0)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(Path ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${Path}/include)
add_subdirectory(${Path}/src)

add_executable(Minor ${Headers} ${Sources})

target_include_directories(Minor PUBLIC ${Path}/include)
