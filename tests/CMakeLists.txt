enable_testing()


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

add_test(
    NAME server_connection
    COMMAND python3
        ${CMAKE_CURRENT_SOURCE_DIR}/networking/integration/test_client.py
        $<TARGET_FILE:karkinolution_app>
)

find_program(GODOT_BIN NAMES godot godot4)
if(GODOT_BIN)
    add_test(
        NAME godot_server_tests
        COMMAND python3
            ${CMAKE_CURRENT_SOURCE_DIR}/godot/test_client.py
            $<TARGET_FILE:karkinolution_app>
            ${GODOT_BIN}
            ${CMAKE_SOURCE_DIR}/src/godot/karkinolution
            ${CMAKE_CURRENT_SOURCE_DIR}/godot/test_karkinolution.gd
    )
endif()