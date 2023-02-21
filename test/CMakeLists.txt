include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

get_filename_component(source_dir ../ ABSOLUTE)

add_executable(stringmaniptest StringManipTest.cpp)
target_include_directories(stringmaniptest PRIVATE "${source_dir}")
target_link_libraries(stringmaniptest GTest::gtest_main stringmanip)
gtest_discover_tests(stringmaniptest)

add_executable(HttpMessageTest HttpMessageTest.cpp)
target_include_directories(HttpMessageTest PRIVATE "${source_dir}")
target_link_libraries(HttpMessageTest GTest::gtest_main stringmanip)
gtest_discover_tests(HttpMessageTest)