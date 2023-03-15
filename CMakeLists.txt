cmake_minimum_required(VERSION 3.0.0)
project(testsocket VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)

include(GoogleTest)
enable_testing()

include_directories(modules/httpmessage modules/stringmanip modules/socket)

add_subdirectory(modules)

add_executable(testsocket main.cpp)
target_link_libraries(testsocket httpmessage socket)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)