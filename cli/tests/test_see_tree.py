import unittest
from pathlib import Path

from karkinolution.see.namespaces.tree import (
    build_tree,
    display_path,
    file_paths,
    filter_ignored,
    is_ignored,
)

ROOT = Path("/repo")


def names(node) -> list[str]:
    return [child.name for child in node.sorted_children()]


class TestBuildTree(unittest.TestCase):
    def setUp(self) -> None:
        self.base = ROOT / "src"
        self.files = [
            self.base / "b.hpp",
            self.base / "a" / "deep" / "x.cpp",
            self.base / "a" / "y.hpp",
        ]

    def test_builds_directories_and_files(self) -> None:
        tree = build_tree(self.base, self.files, None)

        self.assertEqual(names(tree), ["a", "b.hpp"])
        self.assertEqual(names(tree.children["a"]), ["deep", "y.hpp"])
        self.assertTrue(tree.children["a"].is_directory)
        self.assertFalse(tree.children["b.hpp"].is_directory)

    def test_children_are_sorted_alphabetically_ignoring_case_and_mixing_kinds(self) -> None:
        files = [self.base / "Zeta.hpp", self.base / "alpha" / "f.hpp", self.base / "beta.hpp"]

        self.assertEqual(names(build_tree(self.base, files, None)), ["alpha", "beta.hpp", "Zeta.hpp"])

    def test_max_depth_hides_deeper_levels_like_tree_dash_l(self) -> None:
        tree = build_tree(self.base, self.files, 1)

        self.assertEqual(names(tree), ["a", "b.hpp"])
        self.assertEqual(tree.children["a"].children, {})

    def test_max_depth_keeps_directories_that_reach_the_limit(self) -> None:
        tree = build_tree(self.base, self.files, 2)

        self.assertEqual(names(tree.children["a"]), ["deep", "y.hpp"])
        self.assertEqual(tree.children["a"].children["deep"].children, {})
        self.assertTrue(tree.children["a"].children["deep"].is_directory)

    def test_file_paths_lists_only_visible_files_in_display_order(self) -> None:
        tree = build_tree(self.base, self.files, 2)

        self.assertEqual(file_paths(tree), [self.base / "a" / "y.hpp", self.base / "b.hpp"])


class TestIgnore(unittest.TestCase):
    def test_matches_a_file_name_anywhere(self) -> None:
        self.assertTrue(is_ignored(ROOT / "src" / "x" / "alias.hpp", ROOT, ["alias.hpp"]))

    def test_matches_a_glob_on_the_file_name(self) -> None:
        self.assertTrue(is_ignored(ROOT / "src" / "a.cpp", ROOT, ["*.cpp"]))
        self.assertFalse(is_ignored(ROOT / "src" / "a.hpp", ROOT, ["*.cpp"]))

    def test_matching_a_directory_ignores_everything_below_it(self) -> None:
        self.assertTrue(is_ignored(ROOT / "src" / "binary" / "deep" / "f.hpp", ROOT, ["src/binary"]))
        self.assertTrue(is_ignored(ROOT / "src" / "binary" / "f.hpp", ROOT, ["binary"]))

    def test_does_not_match_unrelated_paths(self) -> None:
        self.assertFalse(is_ignored(ROOT / "src" / "binary2" / "f.hpp", ROOT, ["src/binary"]))

    def test_filter_ignored_accepts_a_trailing_slash(self) -> None:
        files = [ROOT / "src" / "binary" / "f.hpp", ROOT / "src" / "core" / "g.hpp"]

        self.assertEqual(filter_ignored(files, ROOT, ["src/binary/"]), [files[1]])


class TestDisplayPath(unittest.TestCase):
    def test_inside_the_repository_the_path_is_relative(self) -> None:
        self.assertEqual(display_path(ROOT / "src" / "a", ROOT), "src/a")

    def test_the_repository_root_itself_is_a_dot(self) -> None:
        self.assertEqual(display_path(ROOT, ROOT), ".")

    def test_outside_the_repository_the_path_stays_absolute(self) -> None:
        self.assertEqual(display_path(Path("/elsewhere/x"), ROOT), "/elsewhere/x")


if __name__ == "__main__":
    unittest.main()
