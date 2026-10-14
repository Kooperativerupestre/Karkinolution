from dataclasses import dataclass, field
from functools import cache

import tree_sitter_cpp
from tree_sitter import Language, Node, Parser

ANONYMOUS = "(anonymous)"

NOT_OWN_CONTENT = {"namespace_definition", "comment"}


@dataclass(frozen=True)
class NamespaceDeclaration:
    name: str
    line: int


@dataclass(frozen=True)
class NamespaceAlias:
    name: str
    target: str
    line: int


@dataclass(frozen=True)
class FileNamespaces:
    declarations: tuple[NamespaceDeclaration, ...]
    aliases: tuple[NamespaceAlias, ...]
    has_syntax_errors: bool


@dataclass
class Collector:
    declarations: list[NamespaceDeclaration] = field(default_factory=list)
    aliases: list[NamespaceAlias] = field(default_factory=list)
    visited_namespaces: int = 0


@cache
def cpp_parser() -> Parser:
    return Parser(Language(tree_sitter_cpp.language()))


def compact(node: Node) -> str:
    return "".join(node.text.decode().split())


def namespace_name(node: Node) -> str:
    name = node.child_by_field_name("name")
    return compact(name) if name is not None else ANONYMOUS


def has_own_content(body: Node) -> bool:
    return any(child.is_named and child.type not in NOT_OWN_CONTENT for child in body.children)


def read_alias(node: Node) -> NamespaceAlias | None:
    name = node.child_by_field_name("name")
    target = next(
        (child for child in node.named_children if name is not None and child.id != name.id and child.type != "comment"),
        None,
    )

    if name is None or target is None:
        return None

    return NamespaceAlias(compact(name), compact(target), node.start_point[0] + 1)


def visit(node: Node, parents: tuple[str, ...], collector: Collector) -> None:
    for child in node.children:
        if child.type == "namespace_definition":
            visit_namespace(child, parents, collector)
        elif child.type == "namespace_alias_definition":
            alias = read_alias(child)
            if alias is not None:
                collector.aliases.append(alias)
        else:
            visit(child, parents, collector)


def visit_namespace(node: Node, parents: tuple[str, ...], collector: Collector) -> None:
    path = (*parents, namespace_name(node))
    collector.visited_namespaces += 1
    visited_before = collector.visited_namespaces
    body = node.child_by_field_name("body")

    if body is not None:
        visit(body, path, collector)

    has_nested_namespaces = collector.visited_namespaces > visited_before
    owns_content = body is not None and has_own_content(body)

    if not has_nested_namespaces or owns_content:
        collector.declarations.append(NamespaceDeclaration("::".join(path), node.start_point[0] + 1))


def parse_namespaces(source: bytes) -> FileNamespaces:
    tree = cpp_parser().parse(source)
    collector = Collector()
    visit(tree.root_node, (), collector)

    return FileNamespaces(
        declarations=tuple(sorted(collector.declarations, key=lambda declaration: declaration.line)),
        aliases=tuple(sorted(collector.aliases, key=lambda alias: alias.line)),
        has_syntax_errors=tree.root_node.has_error,
    )
