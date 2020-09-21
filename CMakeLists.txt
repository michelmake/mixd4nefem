cmake_minimum_required(VERSION 2.8)
project(NefemRenumber)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(NefemRenumber element.cpp main.cpp)
target_link_libraries(NefemRenumber ${Boost_LIBRARIES})
