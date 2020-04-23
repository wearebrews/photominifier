# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(photominifier)


# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)

# I../includes
include_directories(includes)

find_package(AWSSDK COMPONENTS s3)


# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)

# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(main ${SOURCES})

target_link_libraries(main PRIVATE ${AWSSDK_LINK_LIBRARIES})