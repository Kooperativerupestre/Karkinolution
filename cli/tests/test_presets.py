import tempfile
import unittest
from pathlib import Path

import typer

from karkinolution.presets import (
    binary_dir,
    cache_value,
    find_repo_root,
    load_configure_presets,
    resolve_preset,
)
from support import write_minimal_repo

REPOSITORY_ROOT = Path(__file__).resolve().parents[2]


class TestFindRepoRoot(unittest.TestCase):
    def test_finds_root_from_nested_directory(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            root = Path(tmpdir).resolve()
            write_minimal_repo(root)
            nested = root / "src" / "deep"
            nested.mkdir(parents=True)

            self.assertEqual(find_repo_root(nested), root)

    def test_exits_when_presets_file_is_missing(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            with self.assertRaises(typer.Exit):
                find_repo_root(Path(tmpdir))


class TestResolvePreset(unittest.TestCase):
    def test_merges_inherited_fields(self) -> None:
        presets = {
            "base": {"name": "base", "hidden": True, "generator": "Ninja"},
            "child": {
                "name": "child",
                "inherits": "base",
                "cacheVariables": {"A": "1"},
            },
        }

        resolved = resolve_preset("child", presets)

        self.assertEqual(resolved["generator"], "Ninja")
        self.assertEqual(resolved["cacheVariables"], {"A": "1"})
        self.assertNotIn("inherits", resolved)

    def test_child_overrides_parent_cache_variable(self) -> None:
        presets = {
            "parent": {"name": "parent", "hidden": True, "cacheVariables": {"A": "parent", "B": "keep"}},
            "child": {"name": "child", "inherits": "parent", "cacheVariables": {"A": "child"}},
        }

        self.assertEqual(
            resolve_preset("child", presets)["cacheVariables"],
            {"A": "child", "B": "keep"},
        )

    def test_earlier_parent_wins_over_later_parent(self) -> None:
        presets = {
            "first": {"name": "first", "hidden": True, "cacheVariables": {"A": "first"}},
            "second": {"name": "second", "hidden": True, "cacheVariables": {"A": "second", "B": "second"}},
            "child": {"name": "child", "inherits": ["first", "second"]},
        }

        self.assertEqual(
            resolve_preset("child", presets)["cacheVariables"],
            {"A": "first", "B": "second"},
        )

    def test_unknown_preset_exits(self) -> None:
        with self.assertRaises(typer.Exit):
            resolve_preset("missing", {})

    def test_hidden_preset_exits(self) -> None:
        presets = {"base": {"name": "base", "hidden": True}}

        with self.assertRaises(typer.Exit):
            resolve_preset("base", presets)


class TestCacheValue(unittest.TestCase):
    def test_reads_plain_dict_and_boolean_values(self) -> None:
        resolved = {
            "cacheVariables": {
                "PLAIN": "text",
                "TYPED": {"type": "STRING", "value": "typed"},
                "ENABLED": True,
                "DISABLED": False,
            }
        }

        self.assertEqual(cache_value(resolved, "PLAIN"), "text")
        self.assertEqual(cache_value(resolved, "TYPED"), "typed")
        self.assertEqual(cache_value(resolved, "ENABLED"), "ON")
        self.assertEqual(cache_value(resolved, "DISABLED"), "OFF")

    def test_returns_none_for_missing_variable(self) -> None:
        self.assertIsNone(cache_value({}, "ANY"))


class TestBinaryDir(unittest.TestCase):
    def test_expands_source_dir_and_preset_name(self) -> None:
        resolved = {"binaryDir": "${sourceDir}/out/${presetName}"}

        self.assertEqual(
            binary_dir("debug", resolved, Path("/repo")),
            Path("/repo/out/debug"),
        )


class TestRepositoryPresets(unittest.TestCase):
    def setUp(self) -> None:
        self.presets = load_configure_presets(REPOSITORY_ROOT)
        self.visible = [name for name, preset in self.presets.items() if not preset.get("hidden", False)]

    def test_every_visible_preset_has_a_unique_binary_dir(self) -> None:
        directories = [
            binary_dir(name, resolve_preset(name, self.presets), REPOSITORY_ROOT)
            for name in self.visible
        ]

        self.assertEqual(len(directories), len(set(directories)))

    def test_debug_preset_builds_into_build_with_clang(self) -> None:
        resolved = resolve_preset("debug", self.presets)

        self.assertEqual(binary_dir("debug", resolved, REPOSITORY_ROOT), REPOSITORY_ROOT / "build")
        self.assertEqual(cache_value(resolved, "CMAKE_CXX_COMPILER"), "clang++")
        self.assertEqual(cache_value(resolved, "CMAKE_BUILD_TYPE"), "Debug")


if __name__ == "__main__":
    unittest.main()
