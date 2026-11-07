enable_testing()

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/dependencies/googletest"
    "${CMAKE_CURRENT_BINARY_DIR}/googletest"
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/view/gui/panel/panel.cpp$")

add_executable(karkinolution_tests
    ${TEST_SOURCES}
)

target_include_directories(karkinolution_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(karkinolution_tests PRIVATE
    karkinolution
    opsiamethystos
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(karkinolution_tests)

add_executable(opsiamethystos_tests
    "${CMAKE_CURRENT_SOURCE_DIR}/view/gui/panel/panel.cpp"
)

target_include_directories(opsiamethystos_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(opsiamethystos_tests PRIVATE
    opsiamethystos
    karkinolution
    raylib
    GTest::gtest_main
)

gtest_discover_tests(opsiamethystos_tests)
