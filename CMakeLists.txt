cmake_minimum_required(VERSION 3.1.3)
project(hina)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Wextra)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(libsodium
    PREFIX ${CMAKE_BINARY_DIR}/libsodium
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libsodium
    CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/libsodium --disable-pie
    BUILD_IN_SOURCE 1
    TEST_COMMAND make check
    TEST_BEFORE_INSTALL 1
)

option(BUILD_SHARED_LIBS "Build shared libraries." ON)
add_library(hina
    src/random_stream.cpp
    src/slice2d.cpp
)
add_dependencies(hina libsodium)
include_directories(${CMAKE_BINARY_DIR}/libsodium/include)
target_link_libraries(hina ${CMAKE_BINARY_DIR}/libsodium/lib/libsodium.a)

add_executable(test src/test.cpp)
target_link_libraries(test hina)
