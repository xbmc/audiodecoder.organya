cmake_minimum_required(VERSION 3.5)
project(orgyana)

set(SOURCES blip_buf.c
            decoder.cpp
            file.cpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(organya STATIC ${SOURCES})

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(organya PRIVATE -D__inline__=__inline)
endif()
