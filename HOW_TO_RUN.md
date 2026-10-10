# How to Run - Karkinolution

Run every command from the repository root.

## Requirements

* CMake 3.28 or newer
* A C++23 compiler (CI uses `clang-21`; the Valgrind job uses `gcc-15`)
* Ninja
* Git
* Network access on the first configure (see [What CMake downloads for you](#what-cmake-downloads-for-you))

## 1. Get the code

Three dependencies are Git submodules, so a plain `git clone` is not enough:

```bash
git clone --recurse-submodules https://github.com/Kooperativerupestre/karkinolution.git
cd karkinolution
```

Already cloned without `--recurse-submodules`:

```bash
git submodule update --init dependencies/FastNoise2 dependencies/asio
```

`dependencies/godot-cpp` is only needed for the Godot extension (see [Godot extension](#godot-extension)).

## 2. Configure and build

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++

cmake --build build
```

Configure once. After that, `cmake --build build` is enough: `.cpp` files are collected with `CONFIGURE_DEPENDS`, so new
source files are picked up on the next build.

Build a single target:

```bash
cmake --build build --target karkinolution_app
cmake --build build --target karkinolution_tests
```

## 3. Run the server

```bash
./build/karkinolution_app
```

* The server listens on port `9000` ([`port.hpp`](src/karkinolution/networking/port.hpp)).
* It accepts one client at a time and does not stay alive if Godot disconnects.
* More networking questions: [`help.md`](src/karkinolution/networking/help.md).

## 4. Run the tests

```bash
ctest --test-dir build --output-on-failure
ctest --test-dir build -R '<regex>' --output-on-failure
```

## What CMake downloads for you

Some dependencies arrive on their own at configure time, others do not.

| Dependency | How it arrives | Needs a manual step? |
|---|---|---|
| magic_enum `v0.9.8` | `FetchContent` in the root `CMakeLists.txt` | No |
| GoogleTest `v1.17.0` | `FetchContent` in `tests/CMakeLists.txt` | No |
| FastSIMD | Fetched by FastNoise2's own CMake (CPM) | No |
| FastNoise2 | Git submodule | Yes: `git submodule update --init` |
| asio | Git submodule | Yes: `git submodule update --init` |
| godot-cpp | Git submodule, only with `-DBUILD_GODOT_EXTENSION=ON` | Yes, for the Godot extension only |

Downloaded sources are stored in `build/_deps/`, so deleting `build/` means downloading them again.

If `dependencies/FastNoise2` is empty, configure stops with:

```
CMake Error at CMakeLists.txt:22 (add_subdirectory):
  ... does not contain a CMakeLists.txt file.
```

The fix is the `git submodule update --init` command from step 1.

## Godot extension

```bash
git submodule update --init dependencies/godot-cpp

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_GODOT_EXTENSION=ON
cmake --build build --target karkinolution_godot
```

godot-cpp is added with `add_subdirectory`, so it is built inside the same CMake build. No separate `scons` step is
involved. The library is written to `src/godot/karkinolution/bin`.

## Style and tidy checks

The CLI wraps `clang-format` and `clang-tidy`. See [`CLI.md`](CLI.md) for activation.

```bash
uv sync
source .venv/bin/activate

karkinolution style check src/karkinolution main.cpp
karkinolution tidy check src/karkinolution main.cpp --build-dir build
```

Check only the files changed since a Git ref (compared against the merge base):

```bash
karkinolution tidy check src/karkinolution main.cpp --build-dir build --changed-since origin/main
```

`tidy check` needs `build/compile_commands.json`, which is regenerated on every configure.

## More commands

Release builds, sanitizers, Valgrind, Docker Compose and the CI image are listed in [`docs/commands.md`](docs/commands.md).
