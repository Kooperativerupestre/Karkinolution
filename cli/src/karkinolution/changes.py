import subprocess
from pathlib import Path

import typer


def changed_paths(base_ref: str, cwd: Path | None = None) -> set[Path]:
    base_dir = (cwd or Path.cwd()).resolve()

    result = subprocess.run(
        [
            "git",
            "diff",
            "--name-only",
            "--relative",
            "--diff-filter=ACMR",
            "-z",
            f"{base_ref}...HEAD",
        ],
        cwd=base_dir,
        capture_output=True,
        text=True,
        check=False,
    )

    if result.returncode != 0:
        typer.echo(
            f"Could not compute changes since '{base_ref}': {result.stderr.strip()}",
            err=True,
        )
        raise typer.Exit(code=result.returncode)

    return {(base_dir / name).resolve() for name in result.stdout.split("\0") if name}


def only_changed(files: list[Path], changed: set[Path]) -> list[Path]:
    return [file for file in files if file.resolve() in changed]
