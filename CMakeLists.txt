cmake_minimum_required(VERSION 3.28)

project(Karkinolution LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
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

enable_testing()

# ---------------- Dependencies ----------------
# Sanitizer flags are NOT applied here: dependencies stay uninstrumented
# (faster builds, less RAM, and no noise from third-party code).

set(FASTNOISE2_TOOLS OFF CACHE BOOL "" FORCE)

add_subdirectory(
"${CMAKE_SOURCE_DIR}/dependencies/FastNoise2"
)

add_library(asio INTERFACE)

target_include_directories(asio INTERFACE
"${CMAKE_SOURCE_DIR}/dependencies/asio/asio/include"
)

set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MAGIC_ENUM_OPT_BUILD_TESTS    OFF CACHE BOOL "" FORCE)

add_subdirectory(
"${CMAKE_SOURCE_DIR}/dependencies/magic_enum"
)

option(KARKINOLUTION_BUILD_VIEW "Build karkinolution_view target" OFF)

if (KARKINOLUTION_BUILD_VIEW)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(
        "${CMAKE_SOURCE_DIR}/dependencies/raylib"
    )
endif ()

# ---------------- Sanitizers ----------------
# add_compile_options / add_link_options only affect targets created AFTER
# this point, so everything above stays uninstrumented.

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

# ---------------- Library ----------------

file(GLOB_RECURSE KARKINOLUTION_SOURCES CONFIGURE_DEPENDS
"${CMAKE_SOURCE_DIR}/src/karkinolution/*.cpp"
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
"${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(karkinolution
PUBLIC
FastNoise
asio
magic_enum::magic_enum
)
# ---------------- Main executable ----------------

add_executable(karkinolution_app main.cpp)

target_link_libraries(karkinolution_app PRIVATE
karkinolution
)

# ---------------- View executable ----------------

if (KARKINOLUTION_BUILD_VIEW)
    file(GLOB_RECURSE KARKINOLUTION_VIEW_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/karkinolution/view/*.cpp"
    )

    add_executable(karkinolution_view ${KARKINOLUTION_VIEW_SOURCES})

    target_link_libraries(karkinolution_view PRIVATE
        karkinolution
        raylib
    )
endif ()

# ---------------- Tests ----------------

add_subdirectory(tests)