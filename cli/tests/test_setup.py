import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import typer

from karkinolution.setup.checks import (
    Problem,
    declared_submodules,
    missing_tool_problems,
    required_submodules,
    required_tools,
    run_checks,
    submodule_problems,
)
from karkinolution.setup.commands import configure
from support import write_minimal_repo

RESOLVED = {
    "generator": "Ninja",
    "cacheVariables": {"CMAKE_C_COMPILER": "clang", "CMAKE_CXX_COMPILER": "clang++"},
}

RESOLVED_WITH_VIEW = {
    **RESOLVED,
    "cacheVariables": {**RESOLVED["cacheVariables"], "KARKINOLUTION_BUILD_VIEW": True},
}

GITMODULES = """[submodule "dependencies/FastNoise2"]
\tpath = dependencies/FastNoise2
\turl = https://example.com/FastNoise2.git
[submodule "dependencies/asio"]
\tpath = dependencies/asio
\turl = https://example.com/asio.git
"""

VIEW_ONLY_GITMODULES = """[submodule "dependencies/raylib"]
\tpath = dependencies/raylib
\turl = https://example.com/raylib.git
[submodule "dependencies/imgui"]
\tpath = dependencies/imgui
\turl = https://example.com/imgui.git
[submodule "dependencies/rlImGui"]
\tpath = dependencies/rlImGui
\turl = https://example.com/rlImGui.git
"""


def populate(path: Path) -> None:
    path.mkdir(parents=True)
    (path / "file.txt").write_text("content")


class TestTools(unittest.TestCase):
    def test_required_tools_include_cmake_generator_and_compilers(self) -> None:
        self.assertEqual(required_tools(RESOLVED), ["cmake", "ninja", "clang", "clang++"])

    def test_reports_only_missing_tools(self) -> None:
        installed = {"cmake", "ninja", "clang"}

        problems = missing_tool_problems(
            ["cmake", "ninja", "clang", "clang++"],
            which=lambda tool: tool if tool in installed else None,
        )

        self.assertEqual([problem.message for problem in problems], ["'clang++' was not found on PATH."])


class TestSubmodules(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tmpdir.name)
        (self.root / ".gitmodules").write_text(GITMODULES)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    def test_declared_submodules_are_read_from_gitmodules(self) -> None:
        self.assertEqual(
            declared_submodules(self.root),
            ["dependencies/FastNoise2", "dependencies/asio"],
        )

    def test_no_gitmodules_means_no_submodules(self) -> None:
        (self.root / ".gitmodules").unlink()

        self.assertEqual(declared_submodules(self.root), [])

    def test_required_submodules_matches_declared(self) -> None:
        self.assertEqual(
            required_submodules(self.root, RESOLVED),
            ["dependencies/FastNoise2", "dependencies/asio"],
        )

    def test_view_only_submodules_are_skipped_when_the_view_is_off(self) -> None:
        (self.root / ".gitmodules").write_text(GITMODULES + VIEW_ONLY_GITMODULES)

        self.assertEqual(
            required_submodules(self.root, RESOLVED),
            ["dependencies/FastNoise2", "dependencies/asio"],
        )

    def test_view_only_submodules_are_required_when_the_view_is_on(self) -> None:
        (self.root / ".gitmodules").write_text(GITMODULES + VIEW_ONLY_GITMODULES)

        self.assertEqual(
            required_submodules(self.root, RESOLVED_WITH_VIEW),
            [
                "dependencies/FastNoise2",
                "dependencies/asio",
                "dependencies/raylib",
                "dependencies/imgui",
                "dependencies/rlImGui",
            ],
        )

    def test_empty_or_absent_submodule_directories_are_reported_with_exact_fix(self) -> None:
        (self.root / "dependencies" / "FastNoise2").mkdir(parents=True)

        problems = submodule_problems(
            self.root,
            ["dependencies/FastNoise2", "dependencies/asio"],
        )

        self.assertEqual(len(problems), 1)
        self.assertIn("dependencies/FastNoise2", problems[0].message)
        self.assertIn("dependencies/asio", problems[0].message)
        self.assertEqual(
            problems[0].fix,
            "git submodule update --init dependencies/FastNoise2 dependencies/asio",
        )

    def test_populated_submodules_have_no_problems(self) -> None:
        populate(self.root / "dependencies" / "FastNoise2")

        self.assertEqual(submodule_problems(self.root, ["dependencies/FastNoise2"]), [])

    def test_run_checks_combines_tool_and_submodule_problems(self) -> None:
        problems = run_checks(self.root, RESOLVED, which=lambda tool: None)

        self.assertEqual(len(problems), 5)


class TestConfigure(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tmpdir.name).resolve()
        write_minimal_repo(self.root)
        patcher = mock.patch("karkinolution.setup.commands.find_repo_root", return_value=self.root)
        patcher.start()
        self.addCleanup(patcher.stop)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    @mock.patch("karkinolution.setup.commands.subprocess.run")
    @mock.patch("karkinolution.setup.commands.run_checks")
    def test_reports_problems_and_never_runs_any_command(
        self, mock_checks: mock.MagicMock, mock_run: mock.MagicMock
    ) -> None:
        mock_checks.return_value = [Problem("broken", "do this")]

        with self.assertRaises(typer.Exit) as raised:
            configure("debug")

        self.assertEqual(raised.exception.exit_code, 1)
        mock_run.assert_not_called()

    @mock.patch("karkinolution.setup.commands.subprocess.run")
    @mock.patch("karkinolution.setup.commands.run_checks", return_value=[])
    def test_configures_with_the_requested_preset(
        self, mock_checks: mock.MagicMock, mock_run: mock.MagicMock
    ) -> None:
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=0)

        configure("debug")

        mock_run.assert_called_once()
        self.assertEqual(mock_run.call_args[0][0], ["cmake", "--preset", "debug"])
        self.assertEqual(mock_run.call_args[1]["cwd"], self.root)

    @mock.patch("karkinolution.setup.commands.subprocess.run")
    @mock.patch("karkinolution.setup.commands.run_checks", return_value=[])
    def test_propagates_cmake_failure(
        self, mock_checks: mock.MagicMock, mock_run: mock.MagicMock
    ) -> None:
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=3)

        with self.assertRaises(typer.Exit) as raised:
            configure("debug")

        self.assertEqual(raised.exception.exit_code, 3)


if __name__ == "__main__":
    unittest.main()
