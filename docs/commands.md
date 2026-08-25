# Development Commands

Run these commands from the repository root.

## Local development

### Configure a Debug build

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++
```

The test dependency (GoogleTest) is downloaded by CMake through `FetchContent`, so the first configuration requires network access.

### Build everything

```bash
cmake --build build
```

### Build a specific target

```bash
cmake --build build --target karkinolution_app
cmake --build build --target karkinolution_tests
```

### Run the application

```bash
./build/karkinolution_app
```

### Run all tests

```bash
ctest --test-dir build --output-on-failure
```

Alternatively, use the CMake test target:

```bash
cmake --build build --target test
```

### List and run selected tests

```bash
ctest --test-dir build -N
ctest --test-dir build -R '<test-name-or-regex>' --output-on-failure
```

Build the tests first if the test executable is not available:

```bash
cmake --build build --target karkinolution_tests
```

### Rebuild from a clean build directory

```bash
rm -rf build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Only remove the build directory; it contains generated files and compiled artifacts.

### Generate or refresh `compile_commands.json`

`CMAKE_EXPORT_COMPILE_COMMANDS` is enabled in the project configuration. Reconfigure the build when the file is missing or stale:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

## Docker Compose

### Start an interactive development container

```bash
docker compose build development
docker compose run --rm development
```

Inside the container, the source is mounted at `/workspace`:

```bash
cmake --build /workspace/build
ctest --test-dir /workspace/build --output-on-failure
/workspace/build/karkinolution_app
```

The build directory and ccache are stored in named Docker volumes.

### Build and run the test image

```bash
docker compose build test
docker compose run --rm test
```

The test image builds the project while the image is created. Rebuild the image after changing source files:

```bash
docker compose build --no-cache test
docker compose run --rm test
```

### Open a shell in the development service

```bash
docker compose run --rm development bash
```

### Stop Compose services

```bash
docker compose down
```

To remove the named build and ccache volumes as well, use the following only when those cached artifacts are no longer needed:

```bash
docker compose down -v
```

## CI checks in Docker

Build the CI image:

```bash
docker build -f Dockerfile.ci -t karkinolution-ci .
```

Configure and build a CI test tree:

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-ci -G Ninja \
  -DCMAKE_C_COMPILER=clang-20 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_BUILD_TYPE=Debug

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake --build /workspace/build-ci

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  ctest --test-dir /workspace/build-ci --output-on-failure
```

Run the sanitizer configuration and tests:

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-sanitizers -G Ninja \
  -DCMAKE_C_COMPILER=clang-20 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake --build /workspace/build-sanitizers

docker run --rm -v "$PWD:/workspace" \
  -e ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
  -e UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  karkinolution-ci \
  ctest --test-dir /workspace/build-sanitizers --output-on-failure
```

Run Valgrind against the test executable:

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-valgrind -G Ninja \
  -DCMAKE_C_COMPILER=gcc-15 \
  -DCMAKE_CXX_COMPILER=g++-15 \
  -DCMAKE_BUILD_TYPE=Debug

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake --build /workspace/build-valgrind

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  valgrind --leak-check=full --show-leak-kinds=all \
  --track-origins=yes --error-exitcode=1 \
  /workspace/build-valgrind/tests/karkinolution_tests
```
