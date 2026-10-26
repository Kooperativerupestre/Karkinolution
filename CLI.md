The Karkinolution CLI configures and runs CMake presets and provides source-formatting and static-analysis commands.
Run commands from the repository root unless noted otherwise.

## Install

```bash
uv sync
source .venv/bin/activate
```

The activation step is optional if you run commands through `uv run karkinolution ...`.

## Build and run

Configure the default `debug` preset, then build and start the application:

```bash
karkinolution setup
karkinolution run
```

`setup` checks the environment and submodules before configuring. It does not install tools or initialize submodules;
when something is missing, it reports the issue and a suggested fix. `run` requires a configured preset, builds
`karkinolution_app` by default, and starts it.

To use another preset or executable:

```bash
karkinolution setup --preset view-debug
karkinolution run --preset view-debug
karkinolution run --preset debug --target karkinolution_app
```

Presets and their build directories are defined in [`CMakePresets.json`](CMakePresets.json). See
[`HOW_TO_RUN.md`](HOW_TO_RUN.md) for prerequisites and setup details, and [`docs/commands.md`](docs/commands.md) for
the CMake equivalents.

## Code quality

Check or format selected source files:

```bash
karkinolution style check src/karkinolution main.cpp
karkinolution style format src/karkinolution/binary/interpreters
```

Run `clang-tidy` checks or apply its available fixes. Tidy needs a configured build with
`compile_commands.json`; the CLI detects `build` by default and uses `build-view` for paths under `view` when that
database exists. Pass `--build-dir` to select another build directory.

```bash
karkinolution tidy check src/karkinolution main.cpp
karkinolution tidy check src/karkinolution main.cpp --changed-since origin/main
karkinolution tidy fix src/karkinolution/binary/interpreters
```

`--changed-since` limits tidy checks to files changed since the given Git ref, compared with its merge base. For a
combined formatting and tidy check or fix, use `quality check` or `quality fix` with the same paths.

## Inspect namespaces

`karkinolution see namespaces` inspects the current source syntax and prints a file tree with declared namespaces and
namespace aliases. Pass paths to narrow the scan; by default it scans `src` and `tests`. Use `--help` for options such
as depth limits, ignore patterns, and link editor selection. The command parses source syntax; it does not preprocess
files or resolve aliases.

## Help

Show the options for any command or subcommand with:

```bash
karkinolution --help
karkinolution run --help
karkinolution style check --help
```
