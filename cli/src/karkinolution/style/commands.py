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
        ["clang-format", "--dry-run", "--Werror", *files],
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    complete()


def format(paths: tuple[Path, ...]) -> None:
    files = collect_source_files(paths)

    if not files:
        typer.echo("No source files found.")
        return

    result = subprocess.run(
        ["clang-format", "-i", *files],
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    typer.echo(f"Formatted {len(files)} files.")
    complete()
