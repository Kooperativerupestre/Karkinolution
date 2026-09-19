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
)

# ---------------- Godot Extension ----------------

add_library(karkinolution_godot SHARED
    src/godot/extension/register_types.cpp
    src/godot/extension/karkinolution.cpp
    src/godot/extension/binary/frame.cpp
    src/godot/extension/model/creature/creature.cpp
)
target_include_directories(karkinolution_godot PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/godot/extension"
    "${CMAKE_SOURCE_DIR}/src/godot/godot-cpp/include"
    "${CMAKE_SOURCE_DIR}/src/godot/godot-cpp/gen/include"
)

set_target_properties(karkinolution_godot PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/godot/karkinolution/bin"
)

target_link_libraries(karkinolution_godot PRIVATE
    karkinolution
    "${CMAKE_SOURCE_DIR}/src/godot/godot-cpp/bin/libgodot-cpp.linux.template_debug.x86_64.a"
)

# ---------------- Main executable ----------------

add_executable(karkinolution_app main.cpp)

target_link_libraries(karkinolution_app PRIVATE
    karkinolution
)

# ---------------- Tests ----------------

add_subdirectory(tests)