# How to Run - Karkinolution

Run every command from the repository root. This page covers the first run. The full list of commands and presets is in
[`docs/commands.md`](docs/commands.md).

## Requirements

* CMake 3.28 or newer
* Ninja
* Clang 21 with GCC 16's libstdc++ headers and libraries. The presets select
  `clang-21` and configure `-stdlib=libstdc++ --gcc-toolchain=/usr`.
* Git
* [Conan 2.x](https://conan.io/) (`conan>=2.34.0`, available via `pip`, system packages, or `uv`)
* [`uv`](https://docs.astral.sh/uv/) (optional, if using uv for Python/Conan tooling)
* Network access on the first install/configure to fetch Conan packages
* Pass this requirements file [requirements.hpp](src/karkinolution/core/requirements.hpp)

## 1. Get the code

The official project toolchain is C++26, Clang 21, and libstdc++ 16. Verify
that Clang selects GCC 16 before configuring:

```bash
clang++-21 -std=c++26 -E -x c++ /dev/null -v
find /usr/include/c++ -name inplace_vector
printf '#include <inplace_vector>\nint main() { std::inplace_vector<int, 1> values; }\n' | \
  clang++-21 --gcc-toolchain=/usr -stdlib=libstdc++ -std=c++26 -x c++ -fsyntax-only -
```

Clone the repository:

```bash
git clone https://github.com/Kooperativerupestre/karkinolution.git
cd karkinolution
```

## 2. Initialize dependencies with Conan

The project manages external C++ dependencies using [Conan 2](conanfile.py).

Detect your default Conan profile (first time only) and install the packages:

```bash
# Detect default profile if not done yet
conan profile detect --force

# Install dependencies for Debug (used by debug, sanitizers, tsan, view-debug presets)
conan install . --build=missing -s build_type=Debug

# Or for Release (used by release, native presets)
conan install . --build=missing -s build_type=Release
```

> **Using `uv`:**
> If you run Conan in a virtual environment via `uv`:
> ```bash
> uv run conan profile detect --force
> uv run conan install . --build=missing -s build_type=Debug
> ```

Conan generates the toolchain and dependencies files under `build/Debug/generators/conan_toolchain.cmake` (or `build/Release/generators/conan_toolchain.cmake`), which are automatically referenced by `CMakePresets.json`.

## 3. Configure and build

Pick one. Both use the `debug` preset from [`CMakePresets.json`](CMakePresets.json).

**With the CLI** ([`CLI.md`](CLI.md) explains how to activate it):

```bash
karkinolution setup
```

`setup` checks the tools, then configures. If something is missing, it reports the problem and the
exact command to fix it.

**With CMake directly:**

```bash
cmake --preset debug
cmake --build --preset debug
```

Configure once. After that, `cmake --build --preset debug` is enough: `.cpp` files are collected with
`CONFIGURE_DEPENDS`, so new source files are picked up on the next build.

## 4. Run the server

```bash
karkinolution run
```

`run` builds `karkinolution_app` and starts it. It is the same as:

```bash
cmake --build build --target karkinolution_app
./build/karkinolution_app
```

* The server listens on port `9000` ([`port.hpp`](karkinolution/networking/port.hpp)).
* More networking questions: [`help.md`](karkinolution/networking/help.md).

## 5. Run the view

**With the CLI:**

```bash
karkinolution setup --preset view-debug
karkinolution run --preset view-debug
```

**With CMake directly:**

```bash
cmake --preset view-debug
cmake --build --preset view-debug
./build-view/karkinolution_view
```

When building in `release` mode, both `karkinolution_app` and `karkinolution_view` are built:

```bash
cmake --preset release
cmake --build --preset release
./build-release/karkinolution_view
```

## 6. Run the tests

```bash
ctest --preset debug
```

## Dependencies (Conan)

External dependencies are managed through [Conan 2](conanfile.py):

| Dependency | Version | Purpose |
|---|---|---|
| FastNoise2 | `1.1.1` | Noise generation |
| asio | `1.38.2` | Networking and asynchronous I/O |
| GoogleTest | `1.18.0` | Unit and integration testing |
| magic_enum | `0.9.8` | Enum reflection utilities |
| raylib | `6.0` | Graphical view / windowing |

Running `conan install . --build=missing -s build_type=Debug` generates the toolchain files in `build/Debug/generators/` required before running `cmake --preset ...`.

## Style and tidy checks

```bash
karkinolution style check src/karkinolution main.cpp
karkinolution tidy check src/karkinolution main.cpp --build-dir build
```

Check only the files changed since a Git ref (compared against the merge base):

```bash
karkinolution tidy check src/karkinolution main.cpp --build-dir build --changed-since origin/main
```

`tidy check` needs `build/compile_commands.json`, which is regenerated on every configure.
