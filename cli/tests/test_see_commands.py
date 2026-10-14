import contextlib
import io
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import typer
from typer.testing import CliRunner

from karkinolution.__main__ import app
from karkinolution.see.namespaces.commands import show
from support import write_minimal_repo


class TestShow(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tmpdir.name).resolve()
        write_minimal_repo(self.root)

        self.write("src/a.hpp", "namespace A { namespace B { void f(); } }\n")
        self.write("src/sub/c.hpp", "namespace C {}\n")
        self.write("src/sub/deep/d.hpp", "namespace D {}\n")
        self.write("src/main.cpp", "int main() { return 0; }\n")
        self.write("tests/t.cpp", "namespace T {}\n")
        self.write("dependencies/lib.hpp", "namespace Lib {}\n")

        patcher = mock.patch("karkinolution.see.namespaces.commands.find_repo_root", return_value=self.root)
        patcher.start()
        self.addCleanup(patcher.stop)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    def write(self, relative: str, content: str) -> None:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)

    def run_show(self, **kwargs) -> tuple[str, str]:
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            show(**kwargs)
        return out.getvalue(), err.getvalue()

    def test_scans_src_and_tests_by_default(self) -> None:
        out, _ = self.run_show(editor="none")

        self.assertIn("src\n", out)
        self.assertIn("tests\n", out)
        self.assertIn("A::B", out)
        self.assertIn("T\n", out)

    def test_dependencies_are_never_scanned_by_default(self) -> None:
        out, _ = self.run_show(editor="none")

        self.assertNotIn("Lib", out)

    def test_an_explicit_path_replaces_the_defaults(self) -> None:
        out, _ = self.run_show(paths=(self.root / "src" / "sub",), editor="none")

        self.assertTrue(out.startswith("src/sub\n"))
        self.assertIn("C\n", out)
        self.assertNotIn("A::B", out)
        self.assertNotIn("tests", out)

    def test_a_single_file_is_shown_under_its_directory(self) -> None:
        out, _ = self.run_show(paths=(self.root / "src" / "a.hpp",), editor="none")

        self.assertEqual(out.splitlines(), ["src", "└── a.hpp  A::B"])

    def test_max_depth_limits_the_levels(self) -> None:
        out, _ = self.run_show(paths=(self.root / "src",), max_depth=1, editor="none")

        self.assertIn("a.hpp", out)
        self.assertIn("sub", out)
        self.assertNotIn("c.hpp", out)

    def test_ignore_skips_files_and_directories(self) -> None:
        out, _ = self.run_show(
            paths=(self.root / "src",),
            ignore=("sub", "*.cpp"),
            editor="none",
        )

        self.assertIn("a.hpp", out)
        self.assertNotIn("sub", out)
        self.assertNotIn("main.cpp", out)

    def test_files_with_syntax_errors_warn_on_stderr_but_stay_listed(self) -> None:
        self.write("src/broken.hpp", "namespace Broken { void f( }\n")

        out, err = self.run_show(paths=(self.root / "src",), editor="none")

        self.assertIn("broken.hpp", out)
        self.assertIn("Warning: src/broken.hpp has syntax errors", err)

    def test_reports_when_nothing_matches(self) -> None:
        out, _ = self.run_show(paths=(self.root / "src",), ignore=("*",), editor="none")

        self.assertEqual(out.strip(), "No source files found.")

    def test_unknown_editor_exits_before_printing_anything(self) -> None:
        with self.assertRaises(typer.Exit):
            self.run_show(editor="emacs")

    def test_a_missing_path_exits(self) -> None:
        with self.assertRaises(typer.Exit):
            self.run_show(paths=(self.root / "nope",), editor="none")


class TestCliWiring(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tmpdir.name).resolve()
        write_minimal_repo(self.root)
        (self.root / "src").mkdir()
        (self.root / "src" / "a.hpp").write_text("namespace A {}\n")
        patcher = mock.patch("karkinolution.see.namespaces.commands.find_repo_root", return_value=self.root)
        patcher.start()
        self.addCleanup(patcher.stop)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    def test_see_namespaces_runs_through_the_cli(self) -> None:
        result = CliRunner().invoke(app, ["see", "namespaces", "--editor", "none"])

        self.assertEqual(result.exit_code, 0)
        self.assertIn("a.hpp  A", result.output)

    def test_short_options_are_accepted(self) -> None:
        result = CliRunner().invoke(app, ["see", "namespaces", str(self.root / "src"), "-L", "1", "-i", "*.cpp", "--editor", "none"])

        self.assertEqual(result.exit_code, 0)

    def test_editor_can_come_from_the_environment(self) -> None:
        result = CliRunner().invoke(app, ["see", "namespaces"], env={"KARKINOLUTION_EDITOR": "emacs"})

        self.assertNotEqual(result.exit_code, 0)

    def test_max_depth_must_be_positive(self) -> None:
        result = CliRunner().invoke(app, ["see", "namespaces", "-L", "0"])

        self.assertNotEqual(result.exit_code, 0)


if __name__ == "__main__":
    unittest.main()
