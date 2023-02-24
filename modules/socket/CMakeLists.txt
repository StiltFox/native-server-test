add_library(socket Socket.cpp)
target_link_libraries(socket httpmessage)

if(NOT SFSkipTesting EQUAL True)
    add_executable(sockettest SocketTest.cpp)
    target_link_libraries(sockettest GTest::gtest_main socket httpmessage)
    gtest_discover_tests(sockettest)
endif()