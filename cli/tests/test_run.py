import json
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import typer

from karkinolution.run.commands import launch
from support import write_minimal_repo


class TestLaunch(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tmpdir.name).resolve()
        write_minimal_repo(self.root)
        self.build_dir = self.root / "build"
        patcher = mock.patch("karkinolution.run.commands.find_repo_root", return_value=self.root)
        patcher.start()
        self.addCleanup(patcher.stop)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    def configure_build_dir(self) -> None:
        self.build_dir.mkdir()
        (self.build_dir / "CMakeCache.txt").write_text("")

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_refuses_to_run_when_build_dir_is_not_configured(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        with self.assertRaises(typer.Exit):
            launch("debug")

        mock_run.assert_not_called()
        mock_os.execv.assert_not_called()

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_does_not_start_the_server_when_the_build_fails(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        self.configure_build_dir()
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=2)

        with self.assertRaises(typer.Exit) as raised:
            launch("debug")

        self.assertEqual(raised.exception.exit_code, 2)
        mock_os.execv.assert_not_called()

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_builds_the_app_then_starts_it_from_the_repo_root(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        self.configure_build_dir()
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=0)
        executable = str(self.build_dir / "karkinolution_app")

        launch("debug")

        self.assertEqual(
            mock_run.call_args[0][0],
            ["cmake", "--build", str(self.build_dir), "--target", "karkinolution_app"],
        )
        mock_os.chdir.assert_called_once_with(self.root)
        mock_os.execv.assert_called_once_with(executable, [executable])

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_builds_the_app_with_default_preset(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        self.configure_build_dir()
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=0)
        executable = str(self.build_dir / "karkinolution_app")

        launch()

        self.assertEqual(
            mock_run.call_args[0][0],
            ["cmake", "--build", str(self.build_dir), "--target", "karkinolution_app"],
        )
        mock_os.chdir.assert_called_once_with(self.root)
        mock_os.execv.assert_called_once_with(executable, [executable])

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_builds_the_view_target_with_view_preset(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        presets = json.loads((self.root / "CMakePresets.json").read_text(encoding="utf-8"))
        presets["configurePresets"].append({
            "name": "view-debug",
            "inherits": ["base", "clang"],
            "binaryDir": "${sourceDir}/build-view",
            "cacheVariables": {"CMAKE_BUILD_TYPE": "Debug", "KARKINOLUTION_BUILD_VIEW": True},
        })
        (self.root / "CMakePresets.json").write_text(json.dumps(presets), encoding="utf-8")
        view_dir = self.root / "build-view"
        view_dir.mkdir()
        (view_dir / "CMakeCache.txt").write_text("")
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=0)
        executable = str(view_dir / "karkinolution_view")

        launch("view-debug")

        self.assertEqual(
            mock_run.call_args[0][0],
            ["cmake", "--build", str(view_dir), "--target", "karkinolution_view"],
        )
        mock_os.chdir.assert_called_once_with(self.root)
        mock_os.execv.assert_called_once_with(executable, [executable])

    @mock.patch("karkinolution.run.commands.os")
    @mock.patch("karkinolution.run.commands.subprocess.run")
    def test_builds_explicit_target(
        self, mock_run: mock.MagicMock, mock_os: mock.MagicMock
    ) -> None:
        self.configure_build_dir()
        mock_run.return_value = subprocess.CompletedProcess(args=[], returncode=0)
        executable = str(self.build_dir / "custom_target")

        launch("debug", target="custom_target")

        self.assertEqual(
            mock_run.call_args[0][0],
            ["cmake", "--build", str(self.build_dir), "--target", "custom_target"],
        )
        mock_os.chdir.assert_called_once_with(self.root)
        mock_os.execv.assert_called_once_with(executable, [executable])


if __name__ == "__main__":
    unittest.main()
