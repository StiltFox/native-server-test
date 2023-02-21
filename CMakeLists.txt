cmake_minimum_required(VERSION 3.0.0)
project(testsocket VERSION 0.1.0)

add_subdirectory(test)

add_library(stringmanip StringManip.cpp)
add_library(httpmessage HttpMessage.cpp)
target_link_libraries(httpmessage stringmanip)
add_executable(testsocket socket.cpp)
target_link_libraries(testsocket httpmessage)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)