project(audiodecoder.organya)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/lib/liborganya)

add_subdirectory(lib/liborganya)

set(ORG_SOURCES src/OrganyaCodec.cpp
                src/RingBuffer.cpp)

set(DEPLIBS organya)

build_addon(audiodecoder.organya ORG DEPLIBS)

include(CPack)
