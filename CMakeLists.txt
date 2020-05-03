# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(photominifier)

# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)

if(NOT BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(include)

find_package(AWSSDK REQUIRED COMPONENTS s3)
find_package(JPEG REQUIRED)
find_package(CURL REQUIRED)


# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)
file(GLOB BUILD_STATIC_LIBS lib/*.a)

# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(photominifier ${SOURCES})

target_link_libraries(photominifier PRIVATE ${AWSSDK_LINK_LIBRARIES} ${CURL_LINK_LIBRARIES})