cmake_minimum_required(VERSION 3.28)

project(Karkinolution LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

enable_testing()

# ---------------- Dependencies ----------------

set(FASTNOISE2_TOOLS OFF CACHE BOOL "" FORCE)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/dependencies/FastNoise2"
)

add_library(asio INTERFACE)

target_include_directories(asio INTERFACE
    "${CMAKE_SOURCE_DIR}/dependencies/asio/asio/include"
)

# ---------------- Library ----------------

file(GLOB_RECURSE KARKINOLUTION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/karkinolution/*.cpp"
)

file(GLOB_RECURSE KARKINOLUTION_MODULES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/karkinolution/*.cppm"
    "${CMAKE_SOURCE_DIR}/src/karkinolution/*.ixx"
)

add_library(karkinolution)

target_sources(karkinolution
    PRIVATE
        ${KARKINOLUTION_SOURCES}

    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${KARKINOLUTION_MODULES}
)

target_include_directories(karkinolution PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(karkinolution
    PUBLIC
        FastNoise
        asio
)

# ---------------- Main executable ----------------

add_executable(karkinolution_app main.cpp)

target_link_libraries(karkinolution_app PRIVATE
    karkinolution
)

# ---------------- Tests ----------------

add_subdirectory(tests)