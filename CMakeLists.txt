cmake_minimum_required(VERSION 3.5)
project(audiodecoder.organya)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    ${p8-platform_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/lib/liborganya)

add_subdirectory(lib/liborganya)

set(ORG_SOURCES src/OrganyaCodec.cpp
                src/RingBuffer.cpp)

set(ORG_HEADERS src/RingBuffer.h)

set(DEPLIBS ${p8-platform_LIBRARIES} organya)

build_addon(audiodecoder.organya ORG DEPLIBS)

include(CPack)
