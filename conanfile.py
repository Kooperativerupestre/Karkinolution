from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout  # type: ignore


class Karkinolution(ConanFile):
    name = "karkinolution"
    version = "0.1.0"

    settings = "os", "arch", "compiler", "build_type"

    requires = (
        "fastnoise2/1.1.1",
        "asio/1.38.2",
        "gtest/1.18.0",
        "magic_enum/0.9.8",
        "raylib/6.0",
    )

    default_options = {
        "fastnoise2/*:shared": False,
        "gtest/*:shared": False,
        "raylib/*:shared": False,
    }

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.generator = "Ninja"
        tc.user_presets_path = False  # type: ignore
        tc.generate()
