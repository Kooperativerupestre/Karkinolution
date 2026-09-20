import subprocess
from pathlib import Path

import typer

from karkinolution.paths import collect_source_files, complete, find_config_file


def check(paths: tuple[Path, ...], style_file: Path | None = None) -> None:
    files = collect_source_files(paths)

    if not files:
        typer.echo("No source files found.")
        return

    config = style_file or find_config_file(".clang-format")
    cmd = ["clang-format", "--dry-run", "--Werror"]
    if config:
        cmd.append(f"--style=file:{config.resolve()}")
    else:
        cmd.append("--style=file")

    cmd.extend(str(f) for f in files)

    result = subprocess.run(
        cmd,
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    complete()


def format(paths: tuple[Path, ...], style_file: Path | None = None) -> None:
    files = collect_source_files(paths)

    if not files:
        typer.echo("No source files found.")
        return

    config = style_file or find_config_file(".clang-format")
    cmd = ["clang-format", "-i"]
    if config:
        cmd.append(f"--style=file:{config.resolve()}")
    else:
        cmd.append("--style=file")

    cmd.extend(str(f) for f in files)

    result = subprocess.run(
        cmd,
        check=False,
    )

    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    typer.echo(f"Formatted {len(files)} files.")
    complete()
