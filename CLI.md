The Karkinolution CLI sets up the build, runs the server, and verifies style and formatting.

To activate the CLI, run:

```bash
uv sync

source .venv/bin/activate
```

`uv` is preferred in this project for dependency management.

## Commands

| Command | What it does |
|---|---|
| `karkinolution setup [--preset debug]` | Checks the tools and submodules, then configures the build. It only reports problems and prints the fix. It never installs or initializes anything. |
| `karkinolution run [--preset debug]` | Builds `karkinolution_app` and starts the server. Requires `setup` first. |
| `karkinolution style check <paths>` | Checks formatting with `clang-format`. |
| `karkinolution style format <paths>` | Formats files in place. |
| `karkinolution tidy check <paths>` | Runs `clang-tidy`. Add `--changed-since <ref>` to check only files changed since a Git ref. |
| `karkinolution tidy fix <paths>` | Runs `clang-tidy` and applies the available fixes. |

Presets come from [`CMakePresets.json`](CMakePresets.json); the full list is in [`docs/commands.md`](docs/commands.md).
Run `karkinolution <command> --help` for every option.
