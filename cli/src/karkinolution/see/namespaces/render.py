from collections.abc import Mapping
from dataclasses import dataclass, field
from pathlib import Path

from rich.style import Style
from rich.text import Text

from karkinolution.see.links import Linker
from karkinolution.see.namespaces.parser import FileNamespaces
from karkinolution.see.namespaces.tree import Node

GAP = 2

BRANCH = "├── "
LAST_BRANCH = "└── "
GUIDE = "│   "
BLANK = "    "

DECLARATION_COLOR = "green"
ALIAS_COLOR = "cyan"


@dataclass
class Row:
    prefix: str
    continuation: str
    label: Text
    annotations: list[Text] = field(default_factory=list)


def annotations_for(path: Path, namespaces: FileNamespaces | None, linker: Linker) -> list[Text]:
    if namespaces is None:
        return []

    entries = [
        (declaration.line, declaration.name, DECLARATION_COLOR) for declaration in namespaces.declarations
    ] + [
        (alias.line, f"{alias.name} = {alias.target}", ALIAS_COLOR) for alias in namespaces.aliases
    ]

    return [
        Text(text, style=Style(color=color, link=linker.url(path, line)))
        for line, text, color in sorted(entries)
    ]


def collect_rows(
    node: Node,
    prefix: str,
    namespaces: Mapping[Path, FileNamespaces],
    linker: Linker,
    rows: list[Row],
) -> None:
    children = node.sorted_children()

    for index, child in enumerate(children):
        is_last = index == len(children) - 1
        continuation = prefix + (BLANK if is_last else GUIDE)
        branch = prefix + (LAST_BRANCH if is_last else BRANCH)

        if child.is_directory:
            rows.append(Row(branch, continuation, Text(child.name, style="bold blue")))
            collect_rows(child, continuation, namespaces, linker, rows)
        else:
            label = Text(child.name, style=Style(link=linker.url(child.path)))
            annotations = annotations_for(child.path, namespaces.get(child.path), linker)
            rows.append(Row(branch, continuation, label, annotations))


def render_tree(
    root_label: str,
    root: Node,
    namespaces: Mapping[Path, FileNamespaces],
    linker: Linker,
) -> list[Text]:
    rows: list[Row] = []
    collect_rows(root, "", namespaces, linker, rows)

    column = max(
        (len(row.prefix) + len(row.label.plain) + GAP for row in rows if row.annotations),
        default=0,
    )

    lines = [Text(root_label, style="bold")]

    for row in rows:
        line = Text.assemble(row.prefix, row.label)

        if not row.annotations:
            lines.append(line)
            continue

        line.append(" " * (column - len(line.plain)))
        line.append_text(row.annotations[0])
        lines.append(line)

        for extra in row.annotations[1:]:
            padding = " " * (column - len(row.continuation))
            lines.append(Text.assemble(row.continuation, padding, extra))

    return lines
