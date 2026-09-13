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


def collect_source_files(paths: tuple[Path, ...]) -> list[Path]:
    files: list[Path] = []

    for path in paths:
        if not path.exists():
            typer.echo(f"Path does not exist: {path}", err=True)
            raise typer.Exit(code=1)

        if path.is_file():
            if path.suffix in SOURCE_EXTENSIONS:
                files.append(path)

            continue

        files.extend(
            file
            for file in path.rglob("*")
            if file.is_file() and file.suffix in SOURCE_EXTENSIONS
        )

    return files


def complete() -> None:
    typer.echo("🦀")
