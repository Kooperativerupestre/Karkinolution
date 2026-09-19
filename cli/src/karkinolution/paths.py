import os
from pathlib import Path

import typer

SOURCE_EXTENSIONS = {
    ".c",
    ".cc",
    ".cpp",
    ".cppm",
    ".cxx",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
}

FORBIDDEN_DIR_NAMES = {"dependencies", "godot-cpp"}


def is_forbidden_path(path: Path) -> bool:
    resolved = path.resolve()
    return any(part in FORBIDDEN_DIR_NAMES for part in resolved.parts)


def collect_source_files(paths: tuple[Path, ...]) -> list[Path]:
    files: list[Path] = []

    for path in paths:
        if not path.exists():
            typer.echo(f"Path does not exist: {path}", err=True)
            raise typer.Exit(code=1)

        if is_forbidden_path(path):
            typer.echo(
                f"Error: Access to forbidden path is disallowed: {path} (dependencies and godot-cpp are excluded).",
                err=True,
            )
            raise typer.Exit(code=1)

        if path.is_file():
            if path.suffix in SOURCE_EXTENSIONS:
                files.append(path)

            continue

        for root, dirs, filenames in os.walk(path):
            dirs[:] = [d for d in dirs if d not in FORBIDDEN_DIR_NAMES]
            root_path = Path(root)
            for filename in filenames:
                file = root_path / filename
                if file.suffix in SOURCE_EXTENSIONS:
                    files.append(file)

    return files


def complete() -> None:
    typer.echo("🦀")
