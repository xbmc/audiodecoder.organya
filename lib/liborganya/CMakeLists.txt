project(orgyana)

enable_language(C)
enable_language(CXX)

cmake_minimum_required(VERSION 2.6)

set(SOURCES blip_buf.c
            decoder.cpp
            file.cpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(organya STATIC ${SOURCES})
