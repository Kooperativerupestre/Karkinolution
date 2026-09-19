import json
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile

import typer

from karkinolution.paths import collect_source_files, complete, find_config_file

MODULE_FLAGS_TO_REMOVE = {
    "-fmodules-ts",
}

MODULE_PREFIXES_TO_REMOVE = (
    "-fdeps-format=",
    "-fmodule-mapper=",
)


def filter_compile_tokens(tokens: list[str]) -> list[str]:
    new_tokens: list[str] = []
    i = 0
    n = len(tokens)
    while i < n:
        token = tokens[i]

        if token in MODULE_FLAGS_TO_REMOVE:
            i += 1
            continue

        if any(token.startswith(prefix) for prefix in MODULE_PREFIXES_TO_REMOVE):
            i += 1
            continue

        if token in ("-fmodule-mapper", "-fdeps-format"):
            i += 2
            continue

        if token.startswith("-I") and ("dependencies/" in token or "_deps/" in token):
            new_tokens.append("-isystem" + token[2:])
            i += 1
            continue

        if token == "-I" and i + 1 < n and ("dependencies/" in tokens[i + 1] or "_deps/" in tokens[i + 1]):
            new_tokens.append("-isystem")
            new_tokens.append(tokens[i + 1])
            i += 2
            continue

        new_tokens.append(token)
        i += 1

    return new_tokens


def sanitize_compile_commands(source_db_file: Path, target_db_dir: Path) -> Path:
    if not source_db_file.exists():
        typer.echo(f"Compilation database not found at: {source_db_file}", err=True)
        raise typer.Exit(code=1)

    with open(source_db_file, "r", encoding="utf-8") as f:
        entries = json.load(f)

    sanitized_entries = []
    for entry in entries:
        new_entry = dict(entry)
        if "arguments" in new_entry:
            new_entry["arguments"] = filter_compile_tokens(list(new_entry["arguments"]))
        elif "command" in new_entry:
            tokens = shlex.split(new_entry["command"])
            new_entry["command"] = shlex.join(filter_compile_tokens(tokens))
        sanitized_entries.append(new_entry)

    target_db_file = target_db_dir / "compile_commands.json"
    with open(target_db_file, "w", encoding="utf-8") as f:
        json.dump(sanitized_entries, f, indent=2)

    return target_db_file


def run_tidy(
    files: list[Path],
    build_dir: Path = Path("build"),
    fix: bool = False,
    warnings_as_errors: bool = True,
    config_file: Path | None = None,
) -> None:
    if not files:
        typer.echo("No source files found.")
        return

    source_db = build_dir / "compile_commands.json"
    if not source_db.exists():
        typer.echo(f"Compilation database not found at: {source_db}", err=True)
        raise typer.Exit(code=1)

    resolved_config = config_file or find_config_file(".clang-tidy")

    with tempfile.TemporaryDirectory(prefix="karkinolution_tidy_") as tmpdir:
        tmp_path = Path(tmpdir)
        sanitize_compile_commands(source_db, tmp_path)

        if resolved_config and resolved_config.is_file():
            shutil.copy(resolved_config, tmp_path / ".clang-tidy")

        cmd = [
            "clang-tidy",
            "-p",
            str(tmp_path),
        ]
        if resolved_config and resolved_config.is_file():
            cmd.append(f"--config-file={resolved_config.resolve()}")
        if warnings_as_errors:
            cmd.append("--warnings-as-errors=*")
        if fix:
            cmd.append("--fix")

        cmd.extend(str(f) for f in files)

        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            raise typer.Exit(code=result.returncode)

    complete()


def check(
    paths: tuple[Path, ...],
    build_dir: Path = Path("build"),
    warnings_as_errors: bool = True,
    config_file: Path | None = None,
) -> None:
    files = collect_source_files(paths)
    run_tidy(
        files,
        build_dir=build_dir,
        fix=False,
        warnings_as_errors=warnings_as_errors,
        config_file=config_file,
    )


def fix(
    paths: tuple[Path, ...],
    build_dir: Path = Path("build"),
    config_file: Path | None = None,
) -> None:
    files = collect_source_files(paths)
    run_tidy(
        files,
        build_dir=build_dir,
        fix=True,
        config_file=config_file,
    )

