cmake_minimum_required(VERSION 3.20)

project(Karkinolution CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_UNIT_TESTS "Build Catch2 unit tests" ON)
option(BUILD_NETWORKING "Build IXWebSocket + json (server)" OFF)

# ---------------- Library ----------------

file(GLOB_RECURSE KARKINOLUTION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/karkinolution/*.cpp"
)

add_library(karkinolution ${KARKINOLUTION_SOURCES})

target_include_directories(karkinolution PUBLIC
    "${CMAKE_SOURCE_DIR}/third_party/FastNoiseLite"
    "${CMAKE_SOURCE_DIR}/src"
)


# ---------------- Main executable ----------------

add_executable(karkinolution_test main.cpp)

target_link_libraries(karkinolution_test PRIVATE
    karkinolution
)


# ---------------- Smoke executable ----------------

add_executable(smoke
    tools/smoke.cpp
)

target_link_libraries(smoke PRIVATE
    karkinolution
)

target_link_options(smoke PRIVATE -rdynamic -no-pie)
target_compile_options(smoke PRIVATE -fno-pie)


# ---------------- Catch2 (only if requested) ----------------

include(FetchContent)

if(BUILD_UNIT_TESTS)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(Catch2)

    file(GLOB_RECURSE KARKINOLUTION_UNIT_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(karkinolution_unit_tests
        ${KARKINOLUTION_UNIT_TEST_SOURCES}
    )

    target_link_libraries(karkinolution_unit_tests PRIVATE
        karkinolution
        Catch2::Catch2WithMain
    )

    include(CTest)
    include(Catch)
    catch_discover_tests(karkinolution_unit_tests)

endif()


# ---------------- IXWebSocket + nlohmann json (only if requested) ----------------

if(BUILD_NETWORKING)

    FetchContent_Declare(
        IXWebSocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG v11.4.6
        GIT_SHALLOW TRUE
    )

    set(USE_TLS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(IXWebSocket)

    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(json)

endif()


# ---------------- Address Sanitizer ----------------

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(ENABLE_ASAN)

    message(STATUS "Address Sanitizer enabled")

    set(ASAN_TARGETS karkinolution karkinolution_test smoke)
    if(BUILD_UNIT_TESTS)
        list(APPEND ASAN_TARGETS karkinolution_unit_tests)
    endif()

    foreach(target ${ASAN_TARGETS})

        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )

        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )

    endforeach()

endif()