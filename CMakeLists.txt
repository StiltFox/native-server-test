cmake_minimum_required(VERSION 3.0.0)
project(testsocket VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(httpmessage HttpMessage.cpp)
add_executable(testsocket socket.cpp)
target_link_libraries(testsocket httpmessage)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
