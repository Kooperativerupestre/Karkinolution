import subprocess
from pathlib import Path

import typer

from karkinolution.paths import collect_source_files, complete


def check(paths: tuple[Path, ...]) -> None:
    files = collect_source_files(paths)

    if not files:
        typer.echo("No source files found.")
        return

    result = subprocess.run(
        [
            "clang-tidy",
            "-p",
            "build",
            *files,
        ],
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    complete()


def fix(paths: tuple[Path, ...]) -> None:
    files = collect_source_files(paths)

    if not files:
        typer.echo("No source files found.")
        return

    result = subprocess.run(
        [
            "clang-tidy",
            "-p",
            "build",
            "--fix",
            *files,
        ],
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    complete()
