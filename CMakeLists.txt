cmake_minimum_required(VERSION 3.20)

project(Karkinolution CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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


# ---------------- Catch2 ----------------

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
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


# ---------------- IXWebSocket ----------------

FetchContent_Declare(
    IXWebSocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT_TAG v11.4.6
)

set(USE_TLS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(IXWebSocket)


# ---------------- nlohmann json ----------------

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)


# ---------------- Address Sanitizer ----------------

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(ENABLE_ASAN)

    message(STATUS "Address Sanitizer enabled")

    target_compile_options(karkinolution PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
    )

    target_link_options(karkinolution PRIVATE
        -fsanitize=address
    )


    target_compile_options(karkinolution_test PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
    )

    target_link_options(karkinolution_test PRIVATE
        -fsanitize=address
    )


    target_compile_options(karkinolution_unit_tests PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
    )

    target_link_options(karkinolution_unit_tests PRIVATE
        -fsanitize=address
    )

endif()