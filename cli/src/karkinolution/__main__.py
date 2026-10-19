from pathlib import Path

import typer

from karkinolution import quality, run, see, setup, style, tidy

app = typer.Typer(help="Development tools for the Karkinolution project.")

style_app = typer.Typer(help="Format and check source code using clang-format.")

tidy_app = typer.Typer(help="Analyze and fix source code using clang-tidy.")

quality_app = typer.Typer(help="Run all code quality checks and fixes (style + tidy).")

see_app = typer.Typer(help="Inspect the structure of the code.")

app.add_typer(see_app, name="see")
app.add_typer(style_app, name="style")
app.add_typer(tidy_app, name="tidy")
app.add_typer(quality_app, name="quality")


@app.command(
    "setup",
    help="Check the environment and configure the build with a CMake preset.",
)
def setup_command(
    preset: str = typer.Option(
        "debug",
        "--preset",
        "-p",
        help="Configure preset from CMakePresets.json.",
    ),
) -> None:
    setup.configure(preset)


@app.command(
    "run",
    help="Build the server and start it.",
)
def run_command(
    preset: str = typer.Option(
        "godot",
        "--preset",
        "-p",
        help="Configure preset from CMakePresets.json.",
    ),
) -> None:
    run.launch(preset)


@see_app.command(
    "namespaces",
    help="Show a file tree with the namespaces and namespace aliases each file declares.",
)
def see_namespaces(
    paths: list[Path] | None = typer.Argument(
        None,
        help="Files or directories to scan. Default: src and tests.",
    ),
    max_depth: int | None = typer.Option(
        None,
        "--max-depth",
        "-L",
        min=1,
        help="Show at most this many directory levels.",
    ),
    ignore: list[str] | None = typer.Option(
        None,
        "--ignore",
        "-i",
        help="Glob pattern for files or directories to skip. Can be repeated.",
    ),
    editor: str | None = typer.Option(
        None,
        "--editor",
        envvar="KARKINOLUTION_EDITOR",
        help="Editor the links open (vscode, vscode-insiders, vscodium, file, none). Detected when omitted.",
    ),
) -> None:
    see.namespaces.show(
        tuple(paths or ()),
        max_depth=max_depth,
        ignore=tuple(ignore or ()),
        editor=editor,
    )


@style_app.command(
    "check",
    help="Check whether source files are correctly formatted.",
)
def style_check(
    paths: list[Path],
    style_file: Path | None = typer.Option(
        None,
        "--style-file",
        "-s",
        help="Path to .clang-format file.",
    ),
) -> None:
    style.check(tuple(paths), style_file=style_file)


@style_app.command(
    "format",
    help="Format source files in-place.",
)
def style_format(
    paths: list[Path],
    style_file: Path | None = typer.Option(
        None,
        "--style-file",
        "-s",
        help="Path to .clang-format file.",
    ),
) -> None:
    style.format(tuple(paths), style_file=style_file)


@tidy_app.command(
    "check",
    help="Run clang-tidy checks on source files.",
)
def tidy_check(
    paths: list[Path],
    build_dir: Path | None = typer.Option(
        None,
        "--build-dir",
        "-b",
        help="Path to the build directory containing compile_commands.json. Detected when omitted.",
    ),
    warnings_as_errors: bool = typer.Option(
        True,
        "--warnings-as-errors/--no-warnings-as-errors",
        help="Treat warnings as errors.",
    ),
    config_file: Path | None = typer.Option(
        None,
        "--config-file",
        "-c",
        help="Path to .clang-tidy file.",
    ),
    changed_since: str | None = typer.Option(
        None,
        "--changed-since",
        help="Only check files changed since this Git ref (compared against the merge base).",
    ),
) -> None:
    tidy.check(
        tuple(paths),
        build_dir=build_dir,
        warnings_as_errors=warnings_as_errors,
        config_file=config_file,
        changed_since=changed_since,
    )


@tidy_app.command(
    "fix",
    help="Run clang-tidy and apply available fixes.",
)
def tidy_fix(
    paths: list[Path],
    build_dir: Path | None = typer.Option(
        None,
        "--build-dir",
        "-b",
        help="Path to the build directory containing compile_commands.json. Detected when omitted.",
    ),
    config_file: Path | None = typer.Option(
        None,
        "--config-file",
        "-c",
        help="Path to .clang-tidy file.",
    ),
) -> None:
    tidy.fix(tuple(paths), build_dir=build_dir, config_file=config_file)


@quality_app.command(
    "check",
    help="Run style and tidy checks on source files.",
)
def quality_check(
    paths: list[Path],
    build_dir: Path | None = typer.Option(
        None,
        "--build-dir",
        "-b",
        help="Path to the build directory containing compile_commands.json. Detected when omitted.",
    ),
    warnings_as_errors: bool = typer.Option(
        True,
        "--warnings-as-errors/--no-warnings-as-errors",
        help="Treat tidy warnings as errors.",
    ),
    style_file: Path | None = typer.Option(
        None,
        "--style-file",
        "-s",
        help="Path to .clang-format file.",
    ),
    config_file: Path | None = typer.Option(
        None,
        "--config-file",
        "-c",
        help="Path to .clang-tidy file.",
    ),
    changed_since: str | None = typer.Option(
        None,
        "--changed-since",
        help="Only tidy-check files changed since this Git ref (compared against the merge base).",
    ),
) -> None:
    quality.check(
        tuple(paths),
        build_dir=build_dir,
        warnings_as_errors=warnings_as_errors,
        style_file=style_file,
        config_file=config_file,
        changed_since=changed_since,
    )


@quality_app.command(
    "fix",
    help="Apply style formatting and tidy fixes to source files.",
)
def quality_fix(
    paths: list[Path],
    build_dir: Path | None = typer.Option(
        None,
        "--build-dir",
        "-b",
        help="Path to the build directory containing compile_commands.json. Detected when omitted.",
    ),
    style_file: Path | None = typer.Option(
        None,
        "--style-file",
        "-s",
        help="Path to .clang-format file.",
    ),
    config_file: Path | None = typer.Option(
        None,
        "--config-file",
        "-c",
        help="Path to .clang-tidy file.",
    ),
) -> None:
    quality.fix(
        tuple(paths),
        build_dir=build_dir,
        style_file=style_file,
        config_file=config_file,
    )


if __name__ == "__main__":
    app()
