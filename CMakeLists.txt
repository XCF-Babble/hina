cmake_minimum_required(VERSION 3.1.3)
project(hina)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Wextra)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(libsodium
    PREFIX ${CMAKE_SOURCE_DIR}/src/libsodium
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libsodium
    INSTALL_DIR ${CMAKE_BINARY_DIR}/libsodium
    CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=/
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND make DESTDIR=${CMAKE_BINARY_DIR}/libsodium install
    TEST_COMMAND make check
    TEST_BEFORE_INSTALL 1
)

option(BUILD_SHARED_LIBS "Build shared libraries." ON)
add_library(hina
    src/random_stream.cpp
    src/slice2d.cpp
)