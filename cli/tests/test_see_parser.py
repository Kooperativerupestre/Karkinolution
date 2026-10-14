import unittest

from karkinolution.see.namespaces.parser import parse_namespaces


def declared(source: str) -> list[tuple[str, int]]:
    parsed = parse_namespaces(source.encode())
    return [(declaration.name, declaration.line) for declaration in parsed.declarations]


class TestDeclarations(unittest.TestCase):
    def test_nested_blocks_are_reported_as_one_fully_qualified_name(self) -> None:
        source = "namespace A {\n  namespace B {\n    void f();\n  }\n}\n"

        self.assertEqual(declared(source), [("A::B", 2)])

    def test_nested_namespace_shorthand(self) -> None:
        self.assertEqual(declared("namespace A::B::C {\n  int x;\n}\n"), [("A::B::C", 1)])

    def test_sibling_namespaces_are_reported_without_their_empty_parent(self) -> None:
        source = "namespace A {\n  namespace B {}\n  namespace C {}\n}\n"

        self.assertEqual(declared(source), [("A::B", 2), ("A::C", 3)])

    def test_parent_with_its_own_content_is_reported_too(self) -> None:
        source = "namespace A {\n  void own();\n  namespace B {}\n}\n"

        self.assertEqual(declared(source), [("A", 1), ("A::B", 3)])

    def test_comments_are_not_own_content(self) -> None:
        source = "namespace A {\n  // only a comment\n  namespace B {}\n}\n"

        self.assertEqual(declared(source), [("A::B", 3)])

    def test_anonymous_namespace(self) -> None:
        source = "namespace Outer {\n  namespace {\n    int hidden;\n  }\n}\n"

        self.assertEqual(declared(source), [("Outer::(anonymous)", 2)])

    def test_inline_namespace(self) -> None:
        self.assertEqual(declared("inline namespace V1 {}\n"), [("V1", 1)])

    def test_reports_the_line_of_the_namespace_keyword(self) -> None:
        self.assertEqual(declared("\n\n\nnamespace Late {}\n"), [("Late", 4)])

    def test_source_without_namespaces(self) -> None:
        parsed = parse_namespaces(b"int main() { return 0; }\n")

        self.assertEqual(parsed.declarations, ())
        self.assertEqual(parsed.aliases, ())


class TestAliases(unittest.TestCase):
    def test_reads_alias_name_target_and_line(self) -> None:
        source = "\nnamespace CorpseDSI = Karkinolution::Binary::Interpreters::DSI::Corpse;\n"

        alias = parse_namespaces(source.encode()).aliases[0]

        self.assertEqual(alias.name, "CorpseDSI")
        self.assertEqual(alias.target, "Karkinolution::Binary::Interpreters::DSI::Corpse")
        self.assertEqual(alias.line, 2)

    def test_alias_with_a_single_identifier_target(self) -> None:
        alias = parse_namespaces(b"namespace Short = Long;\n").aliases[0]

        self.assertEqual((alias.name, alias.target), ("Short", "Long"))

    def test_aliases_are_not_declarations(self) -> None:
        parsed = parse_namespaces(b"namespace Short = A::B;\n")

        self.assertEqual(parsed.declarations, ())

    def test_alias_declared_inside_a_namespace_is_found(self) -> None:
        parsed = parse_namespaces(b"namespace A {\n  namespace Short = B::C;\n}\n")

        self.assertEqual([alias.name for alias in parsed.aliases], ["Short"])


class TestSyntaxErrors(unittest.TestCase):
    def test_valid_source_has_no_syntax_errors(self) -> None:
        self.assertFalse(parse_namespaces(b"namespace A { void f(); }\n").has_syntax_errors)

    def test_broken_source_is_flagged(self) -> None:
        self.assertTrue(parse_namespaces(b"namespace A { void f( }\n").has_syntax_errors)


if __name__ == "__main__":
    unittest.main()
