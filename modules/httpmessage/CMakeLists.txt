add_library(httpmessage HttpMessage.cpp)
target_link_libraries(httpmessage stringmanip)

if(NOT SFSkipTesting EQUAL True)
    add_executable(httpmessagetest HttpMessageTest.cpp)
    target_link_libraries(httpmessagetest GTest::gtest_main httpmessage)
    gtest_discover_tests(httpmessagetest)
endif()