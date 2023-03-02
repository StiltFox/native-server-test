add_library(stringmanip StringManip.cpp)

if(NOT SFSkipTesting EQUAL True)
    add_executable(stringmaniptest StringManipTest.cpp)
    target_link_libraries(stringmaniptest GTest::gtest_main stringmanip)
    gtest_discover_tests(stringmaniptest)
endif()