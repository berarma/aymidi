cmake_minimum_required(VERSION 3.25)

project(
    aymidi
    VERSION 0.0
    LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

add_subdirectory(dpf)
add_subdirectory(src)

#configure_file(${CMAKE_SOURCE_DIR}/files/manifest.ttl ${CMAKE_SOURCE_DIR}/dist/manifest.ttl COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/files/aymidi.ttl ${CMAKE_SOURCE_DIR}/dist/aymidi.ttl COPYONLY)
