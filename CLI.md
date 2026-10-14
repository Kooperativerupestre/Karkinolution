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
| `karkinolution see namespaces [paths]` | Prints a file tree with the namespaces and namespace aliases each file declares. See [below](#namespace-tree). |
| `karkinolution style check <paths>` | Checks formatting with `clang-format`. |
| `karkinolution style format <paths>` | Formats files in place. |
| `karkinolution tidy check <paths>` | Runs `clang-tidy`. Add `--changed-since <ref>` to check only files changed since a Git ref. |
| `karkinolution tidy fix <paths>` | Runs `clang-tidy` and applies the available fixes. |

## Namespace tree

`karkinolution see namespaces` prints a file tree like `tree`, with the namespaces each file declares next to it:

```
$ karkinolution see namespaces src/karkinolution/binary/deserialization/interpreters/corpse
src/karkinolution/binary/deserialization/interpreters/corpse
├── alias.hpp   CorpseDSI = Karkinolution::Binary::Interpreters::DSI::Corpse
├── corpse.cpp
└── corpse.hpp  Karkinolution::Binary::Interpreters::DSI::Corpse
                Karkinolution::Binary::Interpreters::DSI::Corpse::Models
                Karkinolution::Binary::Interpreters::DSI::Corpse::Request
```

Namespace aliases are shown as `Name = Target`. Anonymous namespaces are shown as `(anonymous)`.

| Option | What it does |
|---|---|
| `[paths]` | Files or directories to scan. Default: `src` and `tests`. |
| `-L`, `--max-depth N` | Show at most `N` directory levels, like `tree -L`. |
| `-i`, `--ignore PATTERN` | Skip files or directories matching a glob (`alias.hpp`, `*.cpp`, `src/binary`). Repeatable. |
| `--editor NAME` | Editor the links open: `vscode`, `vscode-insiders`, `vscodium`, `file` or `none`. Also read from `KARKINOLUTION_EDITOR`. |

In a terminal that supports links, a file name opens the file and a namespace opens its declaration line. The editor is
detected from the terminal: inside the VS Code integrated terminal the links open in VS Code. Anywhere else they are plain
`file://` links that open in the default application, without a line number. Use `--editor` to choose another one.

The command reads the syntax only, using tree-sitter. It does not preprocess the code or resolve aliases. A namespace is
listed when it has no nested namespace, or when it also holds its own declarations. A file tree-sitter cannot parse
cleanly prints a warning on stderr and stays in the tree.

Presets come from [`CMakePresets.json`](CMakePresets.json); the full list is in [`docs/commands.md`](docs/commands.md).
Run `karkinolution <command> --help` for every option.
