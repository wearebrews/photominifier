# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(photominifier)

# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)

if(NOT BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


find_package(JPEG REQUIRED)
find_package(CURL REQUIRED)
find_package(storage_client REQUIRED)

find_library(TURBO_RELEASE turbojpeg)

include_directories(${JPEG_INCLUDE_DIR})
include_directories(include)


# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)

# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(photominifier ${SOURCES})
message(${JPEG_INCLUDE_DIR})
message(${TURBO_RELEASE})

target_link_libraries(photominifier PRIVATE ${CURL_LINK_LIBRARIES} storage_client ${TURBO_RELEASE} -lstdc++fs)