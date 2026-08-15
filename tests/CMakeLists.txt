include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(karkinolution_tests
    ${TEST_SOURCES}
)

target_link_libraries(karkinolution_tests PRIVATE
    karkinolution
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(karkinolution_tests)