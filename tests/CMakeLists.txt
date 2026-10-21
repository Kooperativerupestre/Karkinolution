enable_testing()

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/dependencies/googletest"
    "${CMAKE_CURRENT_BINARY_DIR}/googletest"
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(karkinolution_tests
    ${TEST_SOURCES}
)

target_include_directories(karkinolution_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(karkinolution_tests PRIVATE
    karkinolution
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(karkinolution_tests)