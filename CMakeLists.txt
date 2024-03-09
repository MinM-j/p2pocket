cmake_minimum_required(VERSION 3.28.0)
project(Minor VERSION 1.0.0)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Path ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(Boost 1.83.0 COMPONENTS program_options serialization system REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_subdirectory(${Path}/include)
add_subdirectory(${Path}/src)


set(Debug_files
${Path}/debug.h
${Path}/debug.cpp
)

add_executable(Minor ${Path}/main.cpp  ${Headers} ${Sources} ${Debug_files})

target_include_directories(Minor PUBLIC ${Path}/include)
target_include_directories(Minor PUBLIC ${Path})
TARGET_LINK_LIBRARIES(Minor LINK_PUBLIC ${Boost_LIBRARIES})

add_executable(Bootstrap ${Path}/bootstrap.cpp  ${Headers} ${Sources} ${Debug_files})
target_include_directories(Bootstrap PUBLIC ${Path}/include)
target_include_directories(Bootstrap PUBLIC ${Path})
TARGET_LINK_LIBRARIES(Bootstrap LINK_PUBLIC ${Boost_LIBRARIES})
