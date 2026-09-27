# Development Commands

Run from the repository root. Append `-j$(nproc)` to any build command for parallel compilation.

## Local Development

### Configure (Debug)

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++
```

GoogleTest is fetched automatically on first configure; network access required.

### Build

```bash
cmake --build build
cmake --build build --target karkinolution_app
cmake --build build --target karkinolution_tests
```

### Run

```bash
./build/karkinolution_app
ctest --test-dir build --output-on-failure
ctest --test-dir build -R '<regex>' --output-on-failure
```

### Rebuild from scratch

```bash
rm -rf build && cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug && cmake --build build
```

### Godot extension

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_GODOT_EXTENSION=ON
cmake --build build --target karkinolution_godot
```

### `compile_commands.json`

Regenerated on every reconfigure. Re-run `cmake -S . -B build ...` when stale.

## Release Build

```bash
cmake -S . -B build-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++
cmake --build build-release
ctest --test-dir build-release --output-on-failure
```

### Native-optimized (benchmarking only — not portable)

```bash
cmake -S . -B build-native -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS="-march=native"
cmake --build build-native
```

## Docker Compose

### Development container

```bash
docker compose build development
docker compose run --rm development
```

Inside the container (`/workspace` is the source root):

```bash
cmake --build /workspace/build
ctest --test-dir /workspace/build --output-on-failure
```

Build artifacts and ccache live in named Docker volumes.

### Test image

```bash
docker compose build test
docker compose run --rm test
```

Rebuild after source changes:

```bash
docker compose build --no-cache test && docker compose run --rm test
```

### Other

```bash
docker compose run --rm development bash   # shell
docker compose down                        # stop services
docker compose down -v                     # stop + delete volumes
```

## CI (Docker)

### Build image

```bash
docker build -f Dockerfile.ci -t karkinolution-ci .
```

### Configure, build, test

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-ci -G Ninja \
  -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21 \
  -DCMAKE_BUILD_TYPE=Debug

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake --build /workspace/build-ci

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  ctest --test-dir /workspace/build-ci --output-on-failure
```

### Sanitizers

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-sanitizers -G Ninja \
  -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21 \
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

### Valgrind

```bash
docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake -S /workspace -B /workspace/build-valgrind -G Ninja \
  -DCMAKE_C_COMPILER=gcc-15 -DCMAKE_CXX_COMPILER=g++-15 \
  -DCMAKE_BUILD_TYPE=Debug

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  cmake --build /workspace/build-valgrind

docker run --rm -v "$PWD:/workspace" karkinolution-ci \
  valgrind --leak-check=full --show-leak-kinds=all \
  --track-origins=yes --error-exitcode=1 \
  /workspace/build-valgrind/tests/karkinolution_tests
```
