import fnmatch
from collections.abc import Iterable, Sequence
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class Node:
    name: str
    path: Path
    is_directory: bool
    children: dict[str, "Node"] = field(default_factory=dict)

    def sorted_children(self) -> list["Node"]:
        return sorted(self.children.values(), key=lambda child: (child.name.lower(), child.name))


def display_path(path: Path, repo_root: Path) -> str:
    try:
        return str(path.relative_to(repo_root))
    except ValueError:
        return str(path)


def is_ignored(path: Path, repo_root: Path, patterns: Sequence[str]) -> bool:
    try:
        parts = path.relative_to(repo_root).parts
    except ValueError:
        parts = path.parts

    for index in range(1, len(parts) + 1):
        prefix = "/".join(parts[:index])
        name = parts[index - 1]

        if any(fnmatch.fnmatchcase(prefix, pattern) or fnmatch.fnmatchcase(name, pattern) for pattern in patterns):
            return True

    return False


def filter_ignored(files: Iterable[Path], repo_root: Path, patterns: Sequence[str]) -> list[Path]:
    normalized = [pattern.rstrip("/") for pattern in patterns]
    return [file for file in files if not is_ignored(file, repo_root, normalized)]


def build_tree(root: Path, files: Iterable[Path], max_depth: int | None) -> Node:
    tree = Node(name=root.name, path=root, is_directory=True)

    for file in sorted(files):
        parts = file.relative_to(root).parts
        visible = parts if max_depth is None else parts[:max_depth]
        node = tree

        for index, part in enumerate(visible):
            is_directory = index < len(parts) - 1
            child = Node(part, root.joinpath(*parts[: index + 1]), is_directory)
            node = node.children.setdefault(part, child)

    return tree


def file_paths(node: Node) -> list[Path]:
    paths: list[Path] = []

    for child in node.sorted_children():
        if child.is_directory:
            paths.extend(file_paths(child))
        else:
            paths.append(child.path)

    return paths
