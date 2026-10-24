import json
import tempfile
import unittest
from pathlib import Path

import typer
from unittest import mock

from karkinolution.tidy.commands import check, filter_compile_tokens, run_tidy, sanitize_compile_commands


class TestTidy(unittest.TestCase):
    def test_filter_compile_tokens_removes_module_flags(self) -> None:
        tokens = [
            "/usr/bin/c++",
            "-g",
            "-std=c++26",
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
        self.assertIn("-std=c++26", filtered)
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
                    "command": "/usr/bin/c++ -std=c++26 -fmodules-ts -fdeps-format=p1689r5 -I/workspace/dependencies/asio -c a.cpp",
                    "file": "/workspace/a.cpp",
                },
                {
                    "directory": "/workspace",
                    "arguments": [
                        "/usr/bin/c++",
                        "-std=c++26",
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

    @mock.patch("subprocess.run")
    def test_run_tidy_passes_warnings_as_errors(self, mock_run: mock.MagicMock) -> None:
        mock_run.return_value.returncode = 0
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            source_file = tmp_path / "compile_commands.json"
            source_file.write_text("[]")
            source_code = tmp_path / "main.cpp"
            source_code.write_text("int main() {}")

            run_tidy([source_code], build_dir=tmp_path, warnings_as_errors=True)
            mock_run.assert_called_once()
            called_cmd = mock_run.call_args[0][0]
            self.assertIn("--warnings-as-errors=*", called_cmd)

    @mock.patch("subprocess.run")
    def test_run_tidy_omits_warnings_as_errors_when_disabled(self, mock_run: mock.MagicMock) -> None:
        mock_run.return_value.returncode = 0
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            source_file = tmp_path / "compile_commands.json"
            source_file.write_text("[]")
            source_code = tmp_path / "main.cpp"
            source_code.write_text("int main() {}")

            run_tidy([source_code], build_dir=tmp_path, warnings_as_errors=False)
            mock_run.assert_called_once()
            called_cmd = mock_run.call_args[0][0]
            self.assertNotIn("--warnings-as-errors=*", called_cmd)

    @mock.patch("subprocess.run")
    def test_run_tidy_passes_config_file(self, mock_run: mock.MagicMock) -> None:
        mock_run.return_value.returncode = 0
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            source_file = tmp_path / "compile_commands.json"
            source_file.write_text("[]")
            source_code = tmp_path / "main.cpp"
            source_code.write_text("int main() {}")
            config_file = tmp_path / ".clang-tidy"
            config_file.write_text("Checks: '-*'")

            run_tidy([source_code], build_dir=tmp_path, config_file=config_file)
            mock_run.assert_called_once()
            called_cmd = mock_run.call_args[0][0]
            self.assertIn(f"--config-file={config_file.resolve()}", called_cmd)

    @mock.patch("karkinolution.tidy.commands.run_tidy")
    @mock.patch("karkinolution.tidy.commands.changed_paths")
    def test_check_with_changed_since_runs_only_changed_files(
        self, mock_changed: mock.MagicMock, mock_run_tidy: mock.MagicMock
    ) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            changed_file = base / "changed.cpp"
            untouched_file = base / "untouched.cpp"
            changed_file.write_text("int changed() { return 0; }")
            untouched_file.write_text("int untouched() { return 0; }")
            mock_changed.return_value = {changed_file}

            check((base,), changed_since="origin/main")

            mock_changed.assert_called_once_with("origin/main")
            mock_run_tidy.assert_called_once()
            self.assertEqual(mock_run_tidy.call_args[0][0], [changed_file])

    @mock.patch("karkinolution.tidy.commands.run_tidy")
    @mock.patch("karkinolution.tidy.commands.changed_paths")
    def test_check_with_changed_since_skips_tidy_when_nothing_changed(
        self, mock_changed: mock.MagicMock, mock_run_tidy: mock.MagicMock
    ) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            (base / "untouched.cpp").write_text("int untouched() { return 0; }")
            mock_changed.return_value = set()

            check((base,), changed_since="origin/main")

            mock_run_tidy.assert_not_called()

    @mock.patch("karkinolution.tidy.commands.run_tidy")
    @mock.patch("karkinolution.tidy.commands.changed_paths")
    def test_check_without_changed_since_runs_every_file(
        self, mock_changed: mock.MagicMock, mock_run_tidy: mock.MagicMock
    ) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            first = base / "first.cpp"
            second = base / "second.cpp"
            first.write_text("int first() { return 0; }")
            second.write_text("int second() { return 0; }")

            check((base,))

            mock_changed.assert_not_called()
            self.assertCountEqual(mock_run_tidy.call_args[0][0], [first, second])

    @mock.patch("karkinolution.tidy.commands.run_tidy")
    def test_check_respects_explicit_build_dir_for_all_files(self, mock_run_tidy: mock.MagicMock) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            source_file = base / "src" / "karkinolution" / "app.cpp"
            source_file.parent.mkdir(parents=True)
            source_file.write_text("int app() { return 0; }")

            custom_build = Path("custom_build")
            check((source_file,), build_dir=custom_build)

            mock_run_tidy.assert_called_once()
            self.assertEqual(mock_run_tidy.call_args.kwargs["build_dir"], custom_build)


if __name__ == "__main__":
    unittest.main()
