find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# ---------------- opsiamethystos_tests ----------------

file(GLOB_RECURSE OPSIAMETHYSTOS_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/opsiamethystos/*.cpp"
)

if (OPSIAMETHYSTOS_TEST_SOURCES)
    add_executable(opsiamethystos_tests
        ${OPSIAMETHYSTOS_TEST_SOURCES}
    )

    target_include_directories(opsiamethystos_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/opsiamethystos"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(opsiamethystos_tests PRIVATE
        opsiamethystos
        GTest::gtest_main
    )

    gtest_discover_tests(opsiamethystos_tests
        PROPERTIES LABELS "opsiamethystos"
    )
endif ()

# ---------------- karkinolution_core_tests ----------------

file(GLOB_RECURSE KARKINOLUTION_CORE_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/karkinolution_core/*.cpp"
)

if (KARKINOLUTION_CORE_TEST_SOURCES)
    add_executable(karkinolution_core_tests
        ${KARKINOLUTION_CORE_TEST_SOURCES}
    )

    target_include_directories(karkinolution_core_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/karkinolution_core"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(karkinolution_core_tests PRIVATE
        karkinolution
        GTest::gtest_main
    )

    gtest_discover_tests(karkinolution_core_tests
        PROPERTIES LABELS "karkinolution_core"
    )
endif ()

# ---------------- kshared_tests ----------------

file(GLOB_RECURSE KSHARED_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/kshared/*.cpp"
)

if (KSHARED_TEST_SOURCES)
    add_executable(kshared_tests
        ${KSHARED_TEST_SOURCES}
    )

    target_include_directories(kshared_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/kshared"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(kshared_tests PRIVATE
        karkinolution_shared
        GTest::gtest_main
    )

    gtest_discover_tests(kshared_tests
        PROPERTIES LABELS "kshared"
    )
endif ()

# ---------------- karkinolution_app_tests ----------------

if (KARKINOLUTION_BUILD_APP)
    file(GLOB_RECURSE APP_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
    )

    if (APP_TEST_SOURCES)
        add_executable(karkinolution_app_tests
            ${APP_TEST_SOURCES}
        )

        target_include_directories(karkinolution_app_tests PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/app"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_SOURCE_DIR}"
        )

        target_link_libraries(karkinolution_app_tests PRIVATE
            karkinolution_app_lib
            GTest::gtest_main
        )

        gtest_discover_tests(karkinolution_app_tests
            PROPERTIES LABELS "app"
        )
    endif ()
endif ()
