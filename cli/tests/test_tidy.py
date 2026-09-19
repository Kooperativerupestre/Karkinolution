import json
import tempfile
import unittest
from pathlib import Path

import typer

from karkinolution.tidy.commands import filter_compile_tokens, sanitize_compile_commands


class TestTidy(unittest.TestCase):
    def test_filter_compile_tokens_removes_module_flags(self) -> None:
        tokens = [
            "/usr/bin/c++",
            "-g",
            "-std=c++23",
            "-fPIC",
            "-fmodules-ts",
            "-fmodule-mapper=CMakeFiles/target.dir/file.cpp.o.modmap",
            "-MD",
            "-fdeps-format=p1689r5",
            "-c",
            "file.cpp",
        ]
        filtered = filter_compile_tokens(tokens)

        self.assertNotIn("-fmodules-ts", filtered)
        self.assertNotIn("-fdeps-format=p1689r5", filtered)
        self.assertFalse(any(t.startswith("-fmodule-mapper=") for t in filtered))
        self.assertIn("-std=c++23", filtered)
        self.assertIn("-fPIC", filtered)
        self.assertIn("-c", filtered)

    def test_filter_compile_tokens_removes_split_module_mapper(self) -> None:
        tokens = [
            "/usr/bin/c++",
            "-fmodule-mapper",
            "mapfile.modmap",
            "-c",
            "file.cpp",
        ]
        filtered = filter_compile_tokens(tokens)
        self.assertNotIn("-fmodule-mapper", filtered)
        self.assertNotIn("mapfile.modmap", filtered)
        self.assertIn("-c", filtered)

    def test_filter_compile_tokens_converts_dependency_includes_to_system(self) -> None:
        tokens = [
            "-I/workspace/src",
            "-I/workspace/dependencies/asio/asio/include",
            "-I",
            "/workspace/dependencies/FastNoise2/include",
            "-I/workspace/build/_deps/fastsimd-src/include",
        ]
        filtered = filter_compile_tokens(tokens)

        self.assertIn("-I/workspace/src", filtered)
        self.assertIn("-isystem/workspace/dependencies/asio/asio/include", filtered)
        self.assertIn("-isystem", filtered)
        self.assertIn("/workspace/dependencies/FastNoise2/include", filtered)
        self.assertIn("-isystem/workspace/build/_deps/fastsimd-src/include", filtered)

    def test_sanitize_compile_commands(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            source_file = tmp_path / "compile_commands.json"
            target_dir = tmp_path / "sanitized"
            target_dir.mkdir()

            entries = [
                {
                    "directory": "/workspace",
                    "command": "/usr/bin/c++ -std=c++23 -fmodules-ts -fdeps-format=p1689r5 -I/workspace/dependencies/asio -c a.cpp",
                    "file": "/workspace/a.cpp",
                },
                {
                    "directory": "/workspace",
                    "arguments": [
                        "/usr/bin/c++",
                        "-std=c++23",
                        "-fmodule-mapper=test.modmap",
                        "-c",
                        "b.cpp",
                    ],
                    "file": "/workspace/b.cpp",
                },
            ]

            with open(source_file, "w", encoding="utf-8") as f:
                json.dump(entries, f)

            target_file = sanitize_compile_commands(source_file, target_dir)
            self.assertTrue(target_file.exists())

            with open(target_file, "r", encoding="utf-8") as f:
                sanitized = json.load(f)

            self.assertEqual(len(sanitized), 2)
            self.assertNotIn("-fmodules-ts", sanitized[0]["command"])
            self.assertNotIn("-fdeps-format=p1689r5", sanitized[0]["command"])
            self.assertIn("-isystem", sanitized[0]["command"])
            self.assertFalse(any("modmap" in arg for arg in sanitized[1]["arguments"]))

    def test_sanitize_compile_commands_missing_source(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            missing_source = tmp_path / "missing.json"
            target_dir = tmp_path / "target"
            target_dir.mkdir()

            with self.assertRaises(typer.Exit):
                sanitize_compile_commands(missing_source, target_dir)


if __name__ == "__main__":
    unittest.main()
