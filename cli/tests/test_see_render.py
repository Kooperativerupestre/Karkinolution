import unittest
from pathlib import Path

from rich.console import Console

from karkinolution.see.links import Linker
from karkinolution.see.namespaces.parser import FileNamespaces, NamespaceAlias, NamespaceDeclaration
from karkinolution.see.namespaces.render import render_tree
from karkinolution.see.namespaces.tree import build_tree

BASE = Path("/repo/base")


def declared(*items: tuple[str, int]) -> FileNamespaces:
    return FileNamespaces(
        declarations=tuple(NamespaceDeclaration(name, line) for name, line in items),
        aliases=(),
        has_syntax_errors=False,
    )


def plain(lines) -> str:
    return "\n".join(line.plain for line in lines)


def links(lines) -> list[str]:
    console = Console()
    return [
        segment.style.link
        for line in lines
        for segment in line.render(console)
        if segment.style is not None and segment.style.link
    ]


class TestRenderTree(unittest.TestCase):
    def setUp(self) -> None:
        self.files = [BASE / "a.hpp", BASE / "sub" / "b.hpp", BASE / "sub" / "c.cpp"]
        self.tree = build_tree(BASE, self.files, None)
        self.namespaces = {
            BASE / "a.hpp": declared(("A", 1)),
            BASE / "sub" / "b.hpp": declared(("B::X", 3), ("B::Y", 9)),
            BASE / "sub" / "c.cpp": declared(),
        }

    def test_draws_the_tree_and_aligns_namespaces_in_one_column(self) -> None:
        lines = render_tree("base", self.tree, self.namespaces, Linker("none"))

        self.assertEqual(
            plain(lines),
            "base\n"
            "├── a.hpp      A\n"
            "└── sub\n"
            "    ├── b.hpp  B::X\n"
            "    │          B::Y\n"
            "    └── c.cpp",
        )

    def test_extra_namespaces_keep_the_vertical_guide_of_their_parents(self) -> None:
        namespaces = {BASE / "a.hpp": declared(("A", 1), ("A::B", 2))}
        tree = build_tree(BASE, [BASE / "a.hpp", BASE / "z.hpp"], None)

        lines = render_tree("base", tree, namespaces, Linker("none"))

        self.assertEqual(
            plain(lines),
            "base\n├── a.hpp  A\n│          A::B\n└── z.hpp",
        )

    def test_aliases_are_shown_as_name_equals_target_in_line_order(self) -> None:
        namespaces = {
            BASE / "a.hpp": FileNamespaces(
                declarations=(NamespaceDeclaration("A", 5),),
                aliases=(NamespaceAlias("Short", "A", 1),),
                has_syntax_errors=False,
            )
        }
        tree = build_tree(BASE, [BASE / "a.hpp"], None)

        lines = render_tree("base", tree, namespaces, Linker("none"))

        self.assertEqual(plain(lines), "base\n└── a.hpp  Short = A\n           A")

    def test_files_that_were_not_parsed_have_no_annotation(self) -> None:
        tree = build_tree(BASE, [BASE / "a.hpp"], None)

        self.assertEqual(plain(render_tree("base", tree, {}, Linker("none"))), "base\n└── a.hpp")

    def test_a_tree_without_any_namespace_needs_no_alignment(self) -> None:
        tree = build_tree(BASE, [BASE / "a.hpp", BASE / "b.hpp"], None)

        lines = render_tree("base", tree, {}, Linker("none"))

        self.assertEqual(plain(lines), "base\n├── a.hpp\n└── b.hpp")


class TestRenderLinks(unittest.TestCase):
    def test_file_names_link_to_the_file_and_namespaces_to_their_line(self) -> None:
        tree = build_tree(BASE, [BASE / "a.hpp"], None)
        namespaces = {BASE / "a.hpp": declared(("A", 12))}

        lines = render_tree("base", tree, namespaces, Linker("vscode"))

        self.assertEqual(
            links(lines),
            ["vscode://file/repo/base/a.hpp:1", "vscode://file/repo/base/a.hpp:12"],
        )

    def test_aliases_link_to_the_alias_line(self) -> None:
        namespaces = {
            BASE / "a.hpp": FileNamespaces((), (NamespaceAlias("S", "A", 7),), False)
        }
        tree = build_tree(BASE, [BASE / "a.hpp"], None)

        lines = render_tree("base", tree, namespaces, Linker("vscode"))

        self.assertEqual(links(lines)[-1], "vscode://file/repo/base/a.hpp:7")

    def test_no_links_when_disabled(self) -> None:
        tree = build_tree(BASE, [BASE / "a.hpp"], None)
        namespaces = {BASE / "a.hpp": declared(("A", 1))}

        self.assertEqual(links(render_tree("base", tree, namespaces, Linker("none"))), [])


if __name__ == "__main__":
    unittest.main()
