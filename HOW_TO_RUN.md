# How to Run - Karkinolution

Run every command from the repository root. This page covers the first run. The full list of commands and presets is in
[`docs/commands.md`](docs/commands.md).

## Requirements

* CMake 3.28 or newer
* Ninja
* A C++23 compiler. The presets use `clang` and `clang++` from your `PATH`.
* Git
* [`uv`](https://docs.astral.sh/uv/), only if you use the CLI
* Network access on the first configure (see [What CMake downloads for you](#what-cmake-downloads-for-you))
* Pass this requirements file [requirements.hpp](src/karkinolution/core/requirements.hpp)

## 1. Get the code

Five dependencies are Git submodules, so a plain `git clone` is not enough:

```bash
git clone --recurse-submodules https://github.com/Kooperativerupestre/karkinolution.git
cd karkinolution
```

Already cloned without `--recurse-submodules`:

```bash
git submodule update --init dependencies/FastNoise2 dependencies/asio dependencies/googletest dependencies/magic_enum dependencies/raylib
```

## 2. Configure and build

Pick one. Both use the `debug` preset from [`CMakePresets.json`](CMakePresets.json).

**With the CLI** ([`CLI.md`](CLI.md) explains how to activate it):

```bash
karkinolution setup
```

`setup` checks the tools and submodules, then configures. If something is missing, it only reports the problem and the
exact command to fix it. It never installs or initializes anything for you.

**With CMake directly:**

```bash
cmake --preset debug
cmake --build --preset debug
```

Configure once. After that, `cmake --build --preset debug` is enough: `.cpp` files are collected with
`CONFIGURE_DEPENDS`, so new source files are picked up on the next build.

## 3. Run the server

```bash
karkinolution run
```

`run` builds `karkinolution_app` and starts it. It is the same as:

```bash
cmake --build build --target karkinolution_app
./build/karkinolution_app
```

* The server listens on port `9000` ([`port.hpp`](src/karkinolution/networking/port.hpp)).
* It accepts one client at a time and does not stay alive if it disconnects.
* More networking questions: [`help.md`](src/karkinolution/networking/help.md).

## 4. Run the view

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

## 5. Run the tests

```bash
ctest --preset debug
```

## What CMake downloads for you

Some dependencies arrive on their own at configure time, others do not.

| Dependency | How it arrives | Needs a manual step? |
|---|---|---|
| FastSIMD | Fetched by FastNoise2's own CMake (CPM) | No |
| FastNoise2 | Git submodule | Yes: `git submodule update --init` |
| asio | Git submodule | Yes: `git submodule update --init` |
| GoogleTest `v1.17.0` | Git submodule | Yes: `git submodule update --init` |
| magic_enum `v0.9.8` | Git submodule | Yes: `git submodule update --init` |
| raylib | Git submodule | Yes: `git submodule update --init` (needed for `view-debug`, `release`, and `native`) |

Downloaded sources are stored in `build/_deps/`, so deleting `build/` means downloading them again.

If `dependencies/FastNoise2` is empty, `cmake --preset debug` stops with:

```
CMake Error at CMakeLists.txt:22 (add_subdirectory):
  ... does not contain a CMakeLists.txt file.
```

`karkinolution setup` catches this earlier and prints the `git submodule update --init` command to run.

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
