cmake_minimum_required(VERSION 3.28)

project(Karkinolution LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(SANITIZER "" CACHE STRING "Sanitizer to enable: address | thread | (empty = none)")
set_property(CACHE SANITIZER PROPERTY STRINGS "" address thread)

option(ENABLE_ASAN "Shortcut for -DSANITIZER=address" OFF)

if (ENABLE_ASAN AND SANITIZER STREQUAL "")
    set(SANITIZER "address")
endif ()

option(KARKINOLUTION_BUILD_VIEW "Build karkinolution_view target" OFF)

enable_testing()

# ---------------- Dependencies ----------------

find_package(FastNoise2 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)

# ---------------- Sanitizers ----------------

if (SANITIZER STREQUAL "address")
    set(_san_flags -fsanitize=address,undefined)
elseif (SANITIZER STREQUAL "thread")
    set(_san_flags -fsanitize=thread)
elseif (NOT SANITIZER STREQUAL "")
    message(FATAL_ERROR "SANITIZER must be 'address', 'thread' or empty (got '${SANITIZER}')")
endif ()

if (DEFINED _san_flags)
    message(STATUS "Sanitizer enabled: ${SANITIZER}")
    add_compile_options(${_san_flags} -fno-omit-frame-pointer -g)
    add_link_options(${_san_flags})
endif ()

# ---------------- Shared library ----------------

file(GLOB_RECURSE KARKINOLUTION_SHARED_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/shared/*.cpp"
)

add_library(karkinolution_shared)

set_target_properties(karkinolution_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_sources(karkinolution_shared
    PRIVATE
        ${KARKINOLUTION_SHARED_SOURCES}
)

target_include_directories(karkinolution_shared PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(karkinolution_shared
    PUBLIC
        magic_enum::magic_enum
)

# ---------------- Opsiamethystos library ----------------

file(GLOB_RECURSE OPSIAMETHYSTOS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/opsiamethystos/*.cpp"
)

add_library(opsiamethystos)

set_target_properties(opsiamethystos PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_sources(opsiamethystos
    PRIVATE
        ${OPSIAMETHYSTOS_SOURCES}
)

target_include_directories(opsiamethystos PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(opsiamethystos
    PUBLIC
        karkinolution_shared
        raylib
)

# ---------------- Library ----------------

file(GLOB_RECURSE KARKINOLUTION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/karkinolution/*.cpp"
)

list(FILTER KARKINOLUTION_SOURCES EXCLUDE REGEX ".*/view/.*")

add_library(karkinolution)

set_target_properties(karkinolution PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_sources(karkinolution
    PRIVATE
        ${KARKINOLUTION_SOURCES}
)

target_include_directories(karkinolution PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(karkinolution
    PUBLIC
        FastNoise2::FastNoise
        asio::asio
        magic_enum::magic_enum
        karkinolution_shared
)

# ---------------- Main executable ----------------

add_executable(karkinolution_app main.cpp)

target_link_libraries(karkinolution_app PRIVATE
    karkinolution
)

# ---------------- View executable ----------------

if (KARKINOLUTION_BUILD_VIEW)
    file(GLOB_RECURSE KARKINOLUTION_VIEW_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/karkinolution/view/*.cpp"
    )

    list(FILTER KARKINOLUTION_VIEW_SOURCES
        EXCLUDE REGEX ".*/panel/layout/layout.cpp"
    )

    add_executable(karkinolution_view
        ${KARKINOLUTION_VIEW_SOURCES}
    )

    target_link_libraries(karkinolution_view PRIVATE
        karkinolution
        opsiamethystos
        raylib
    )
endif ()

# ---------------- Tests ----------------

add_subdirectory(tests)