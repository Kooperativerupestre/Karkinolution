cmake_minimum_required(VERSION 3.28)

project(Karkinolution LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(BUILD_GODOT_EXTENSION "Build the Godot GDExtension" OFF)

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

include(FetchContent)

set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MAGIC_ENUM_OPT_BUILD_TESTS    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        v0.9.8
)
FetchContent_MakeAvailable(magic_enum)

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
        magic_enum::magic_enum
)
# ---------------- Godot Extension ----------------

if (BUILD_GODOT_EXTENSION)
    set(GODOTCPP_API_VERSION "4.7" CACHE STRING "" FORCE) 
    set(GODOTCPP_TARGET "template_debug" CACHE STRING "" FORCE)
    set(GODOTCPP_DISABLE_EXCEPTIONS OFF CACHE BOOL "" FORCE)
    set(GODOTCPP_BUILD_PROFILE "${CMAKE_SOURCE_DIR}/src/godot/build_profile.json"
        CACHE PATH "" FORCE)

    add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/godot-cpp" godot-cpp)

        file(GLOB_RECURSE KARKINOLUTION_GODOT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/godot/extension/*.cpp"
        )

        add_library(karkinolution_godot SHARED ${KARKINOLUTION_GODOT_SOURCES})

    target_include_directories(karkinolution_godot PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/godot/extension"
    )

    get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)

    set_target_properties(karkinolution_godot PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/godot/karkinolution/bin"
        PREFIX "lib"
        OUTPUT_NAME "karkinolution_godot${GODOTCPP_SUFFIX}"
    )

    target_link_libraries(karkinolution_godot PRIVATE
        karkinolution
        godot-cpp
    )
endif ()
# ---------------- Main executable ----------------

add_executable(karkinolution_app main.cpp)

target_link_libraries(karkinolution_app PRIVATE
        karkinolution
)

# ---------------- Tests ----------------

add_subdirectory(tests)