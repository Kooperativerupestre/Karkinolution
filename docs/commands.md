# Development Commands

Run from the repository root. Builds are configured through [`CMakePresets.json`](../CMakePresets.json).
Append `--parallel $(nproc)` to any build command for parallel compilation.

## Presets

| Preset | Build directory | Purpose |
|---|---|---|
| `debug` | `build` | Everyday development (`clang`) |
| `release` | `build-release` | Optimized build |
| `native` | `build-native` | Optimized with `-march=native`. Benchmarking only, not portable. |
| `sanitizers` | `build-sanitizers` | Debug with AddressSanitizer and UBSan |
| `ci-tidy`, `ci`, `ci-sanitizers`, `ci-tsan` | `build-tidy`, `build-ci`, `build-ci-sanitizers`, `build-ci-tsan` | Used by CI with `clang-21` and ccache |

List them with `cmake --list-presets`. Every preset has a matching build preset. `debug`, `release`, `sanitizers`, `ci` and
`ci-sanitizers` also have a test preset.

## Local Development

### Configure and build (Debug)

```bash
cmake --preset debug
cmake --build --preset debug
```

FastNoise2, asio, GoogleTest, and magic_enum are submodules; see [`HOW_TO_RUN.md`](../HOW_TO_RUN.md).

### Run

```bash
./build/karkinolution_app
ctest --preset debug
ctest --preset debug -R '<regex>'
```

`karkinolution run` builds and starts the server in one step ([`CLI.md`](../CLI.md)).

### Build a single target

```bash
cmake --build build --target karkinolution_app
cmake --build build --target karkinolution_tests
```

### Rebuild from scratch

```bash
rm -rf build && cmake --preset debug && cmake --build --preset debug
```

### `compile_commands.json`

Regenerated on every reconfigure. Re-run `cmake --preset debug` when stale.

## Release Build

```bash
cmake --preset release
cmake --build --preset release
ctest --preset release
```

### Native-optimized (benchmarking only - not portable)

```bash
cmake --preset native
cmake --build --preset native
```

### Sanitizers (AddressSanitizer and UBSan)

```bash
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers
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

The CI presets use `clang-21` and ccache, which are installed in the image.

```bash
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci cmake --preset ci
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci cmake --build --preset ci
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci ctest --preset ci
```

### Sanitizers

The sanitizer options are part of the preset, so no extra environment variables are needed.

```bash
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci cmake --preset ci-sanitizers
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci cmake --build --preset ci-sanitizers
docker run --rm -v "$PWD:/workspace" -w /workspace karkinolution-ci ctest --preset ci-sanitizers
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
