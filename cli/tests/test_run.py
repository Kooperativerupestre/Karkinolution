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


if __name__ == "__main__":
    unittest.main()
