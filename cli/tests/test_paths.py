import tempfile
import unittest
from pathlib import Path

import typer

from karkinolution.paths import collect_source_files, is_forbidden_path


class TestPaths(unittest.TestCase):
    def test_is_forbidden_path(self) -> None:
        self.assertTrue(is_forbidden_path(Path("dependencies")))
        self.assertTrue(is_forbidden_path(Path("dependencies/asio")))
        self.assertTrue(is_forbidden_path(Path("src/godot/godot-cpp")))
        self.assertTrue(is_forbidden_path(Path("src/godot/godot-cpp/include")))

        self.assertFalse(is_forbidden_path(Path("src/karkinolution")))
        self.assertFalse(is_forbidden_path(Path("src/godot/extension")))
        self.assertFalse(is_forbidden_path(Path("main.cpp")))

    def test_collect_source_files_rejects_forbidden_path(self) -> None:
        with self.assertRaises(typer.Exit):
            collect_source_files((Path("dependencies"),))

    def test_collect_source_files_rejects_godot_cpp(self) -> None:
        with self.assertRaises(typer.Exit):
            collect_source_files((Path("src/godot/godot-cpp"),))

    def test_collect_source_files_prunes_forbidden_subdirectories(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir)
            allowed_dir = base / "src"
            allowed_dir.mkdir()
            (allowed_dir / "file.cpp").write_text("int main() {}")

            forbidden_dep = base / "dependencies"
            forbidden_dep.mkdir()
            (forbidden_dep / "dep.cpp").write_text("int dep() {}")

            forbidden_godot = base / "godot-cpp"
            forbidden_godot.mkdir()
            (forbidden_godot / "godot.cpp").write_text("int godot() {}")

            collected = collect_source_files((base,))
            collected_names = [f.name for f in collected]

            self.assertIn("file.cpp", collected_names)
            self.assertNotIn("dep.cpp", collected_names)
            self.assertNotIn("godot.cpp", collected_names)


if __name__ == "__main__":
    unittest.main()
