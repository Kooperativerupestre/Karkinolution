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
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(karkinolution_tests)

if (KARKINOLUTION_BUILD_VIEW)
    add_executable(karkinolution_gui_tests
        "${CMAKE_CURRENT_SOURCE_DIR}/view/gui/panel/panel.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/components/low_level/circle.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/components/low_level/line.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/components/low_level/rectangle.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/components/low_level/text.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/drivers/font.cpp"
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/gui/panel/panel.cpp"
    )

    target_include_directories(karkinolution_gui_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_link_libraries(karkinolution_gui_tests PRIVATE
        karkinolution
        raylib
        GTest::gtest_main
    )

    gtest_discover_tests(karkinolution_gui_tests)
endif ()
