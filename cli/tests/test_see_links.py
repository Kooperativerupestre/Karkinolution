import unittest
from pathlib import Path

import typer

from karkinolution.see.links import Linker, detect_editor, resolve_editor

FILE = Path("/repo/src/a.hpp")


class TestEditorDetection(unittest.TestCase):
    def test_vscode_integrated_terminal_is_detected(self) -> None:
        self.assertEqual(detect_editor({"TERM_PROGRAM": "vscode"}), "vscode")

    def test_any_other_terminal_falls_back_to_plain_file_links(self) -> None:
        self.assertEqual(detect_editor({"TERM_PROGRAM": "konsole"}), "file")
        self.assertEqual(detect_editor({}), "file")

    def test_an_explicit_choice_wins_over_detection(self) -> None:
        self.assertEqual(resolve_editor("none", {"TERM_PROGRAM": "vscode"}), "none")

    def test_without_a_choice_the_editor_is_detected(self) -> None:
        self.assertEqual(resolve_editor(None, {"TERM_PROGRAM": "vscode"}), "vscode")

    def test_unknown_editor_exits(self) -> None:
        with self.assertRaises(typer.Exit):
            resolve_editor("emacs", {})


class TestLinker(unittest.TestCase):
    def test_vscode_link_carries_the_line(self) -> None:
        self.assertEqual(Linker("vscode").url(FILE, 16), "vscode://file/repo/src/a.hpp:16")

    def test_line_defaults_to_the_top_of_the_file(self) -> None:
        self.assertEqual(Linker("vscode").url(FILE), "vscode://file/repo/src/a.hpp:1")

    def test_other_vscode_flavours_use_their_own_scheme(self) -> None:
        self.assertEqual(Linker("vscode-insiders").url(FILE, 3), "vscode-insiders://file/repo/src/a.hpp:3")
        self.assertEqual(Linker("vscodium").url(FILE, 3), "vscodium://file/repo/src/a.hpp:3")

    def test_file_links_have_no_line(self) -> None:
        self.assertEqual(Linker("file").url(FILE, 16), "file:///repo/src/a.hpp")

    def test_none_disables_links(self) -> None:
        self.assertIsNone(Linker("none").url(FILE, 16))

    def test_special_characters_in_paths_are_encoded(self) -> None:
        url = Linker("vscode").url(Path("/my repo/a b.hpp"), 2)

        self.assertEqual(url, "vscode://file/my%20repo/a%20b.hpp:2")


if __name__ == "__main__":
    unittest.main()
