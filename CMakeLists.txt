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

option(KARKINOLUTION_BUILD_APP "Build karkinolution_app target" OFF)

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

add_library(kshared)

set_target_properties(kshared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_sources(kshared
    PRIVATE
        ${KARKINOLUTION_SHARED_SOURCES}
)

target_include_directories(kshared PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(kshared
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
        kshared
        raylib
)

# ---------------- Library ----------------

file(GLOB_RECURSE KARKINOLUTION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/karkinolution/*.cpp"
)

list(FILTER KARKINOLUTION_SOURCES EXCLUDE REGEX ".*/karkinolution/(server|client)\\.cpp$")

add_library(karkinolution_core)

set_target_properties(karkinolution_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_sources(karkinolution_core
    PRIVATE
        ${KARKINOLUTION_SOURCES}
)

target_include_directories(karkinolution_core PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(karkinolution_core
    PUBLIC
        FastNoise2::FastNoise
        asio::asio
        magic_enum::magic_enum
        kshared
)

# ---------------- Server executable ----------------

add_executable(karkinolution_server karkinolution/server.cpp)

target_link_libraries(karkinolution_server PRIVATE
    karkinolution_core
    kshared
)

# ---------------- Client executable ----------------

add_executable(karkinolution_client karkinolution/client.cpp)

target_link_libraries(karkinolution_client PRIVATE
    karkinolution_core
    kshared
)

# ---------------- Application library and executable ----------------

if (KARKINOLUTION_BUILD_APP)
    file(GLOB_RECURSE KARKINOLUTION_APP_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/app/*.cpp"
    )

    list(FILTER KARKINOLUTION_APP_SOURCES EXCLUDE REGEX ".*/main.cpp$")

    add_library(karkinolution_app_lib)

    set_target_properties(karkinolution_app_lib PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )

    target_sources(karkinolution_app_lib
        PRIVATE
            ${KARKINOLUTION_APP_SOURCES}
    )

    target_include_directories(karkinolution_app_lib PUBLIC
        "${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(karkinolution_app_lib
        PUBLIC
            karkinolution_core
            opsiamethystos
            raylib
    )

    add_executable(karkinolution_app app/main.cpp)

    target_link_libraries(karkinolution_app PRIVATE
        karkinolution_app_lib
    )
endif ()

# ---------------- Tests ----------------

add_subdirectory(tests)
