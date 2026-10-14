import os
from pathlib import Path

import typer
from rich.console import Console

from karkinolution.paths import collect_source_files
from karkinolution.presets import find_repo_root
from karkinolution.see.links import Linker, resolve_editor
from karkinolution.see.namespaces.parser import FileNamespaces, parse_namespaces
from karkinolution.see.namespaces.render import render_tree
from karkinolution.see.namespaces.tree import build_tree, display_path, file_paths, filter_ignored

DEFAULT_DIRECTORIES = ("src", "tests")


def resolve_scan_paths(paths: tuple[Path, ...], repo_root: Path) -> list[Path]:
    if paths:
        return [path.resolve() for path in paths]

    return [repo_root / name for name in DEFAULT_DIRECTORIES if (repo_root / name).is_dir()]


def read_namespaces(files: list[Path], repo_root: Path) -> dict[Path, FileNamespaces]:
    namespaces: dict[Path, FileNamespaces] = {}

    for file in files:
        parsed = parse_namespaces(file.read_bytes())

        if parsed.has_syntax_errors:
            typer.echo(
                f"Warning: {display_path(file, repo_root)} has syntax errors; its namespaces may be incomplete.",
                err=True,
            )

        namespaces[file] = parsed

    return namespaces


def show(
    paths: tuple[Path, ...] = (),
    max_depth: int | None = None,
    ignore: tuple[str, ...] = (),
    editor: str | None = None,
) -> None:
    repo_root = find_repo_root()
    linker = Linker(resolve_editor(editor, os.environ))
    console = Console(highlight=False)
    printed = False

    for scan_path in resolve_scan_paths(paths, repo_root):
        files = filter_ignored(collect_source_files((scan_path,)), repo_root, ignore)

        if not files:
            continue

        base = scan_path if scan_path.is_dir() else scan_path.parent
        tree = build_tree(base, files, max_depth)
        namespaces = read_namespaces(file_paths(tree), repo_root)

        if printed:
            console.print()

        for line in render_tree(display_path(base, repo_root), tree, namespaces, linker):
            console.print(line, soft_wrap=True)

        printed = True

    if not printed:
        typer.echo("No source files found.")
