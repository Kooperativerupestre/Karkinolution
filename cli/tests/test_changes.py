import subprocess
import tempfile
import unittest
from pathlib import Path

import typer

from karkinolution.changes import changed_paths, only_changed


def git(repo: Path, *args: str) -> str:
    result = subprocess.run(
        [
            "git",
            "-c",
            "user.name=test",
            "-c",
            "user.email=test@example.com",
            "-c",
            "commit.gpgsign=false",
            *args,
        ],
        cwd=repo,
        capture_output=True,
        text=True,
        check=True,
    )
    return result.stdout.strip()


def commit_all(repo: Path, message: str) -> None:
    git(repo, "add", "-A")
    git(repo, "commit", "-m", message)


class TestChangedPaths(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmpdir.name).resolve()
        git(self.repo, "init", "-q")

        (self.repo / "src").mkdir()
        (self.repo / "src" / "kept.cpp").write_text("int kept() { return 0; }\n")
        (self.repo / "src" / "modified.cpp").write_text("int modified() { return 0; }\n")
        (self.repo / "src" / "deleted.cpp").write_text("int deleted() { return 0; }\n")
        commit_all(self.repo, "initial")
        self.base = git(self.repo, "rev-parse", "HEAD")

    def tearDown(self) -> None:
        self.tmpdir.cleanup()

    def test_lists_added_and_modified_files(self) -> None:
        (self.repo / "src" / "modified.cpp").write_text("int modified() { return 1; }\n")
        (self.repo / "src" / "added.hpp").write_text("#pragma once\n")
        commit_all(self.repo, "change")

        changed = changed_paths(self.base, cwd=self.repo)

        self.assertEqual(
            changed,
            {
                self.repo / "src" / "modified.cpp",
                self.repo / "src" / "added.hpp",
            },
        )

    def test_excludes_deleted_files(self) -> None:
        (self.repo / "src" / "deleted.cpp").unlink()
        commit_all(self.repo, "delete")

        self.assertEqual(changed_paths(self.base, cwd=self.repo), set())

    def test_includes_renamed_files_under_their_new_name(self) -> None:
        git(self.repo, "mv", "src/kept.cpp", "src/renamed.cpp")
        commit_all(self.repo, "rename")

        self.assertEqual(
            changed_paths(self.base, cwd=self.repo),
            {self.repo / "src" / "renamed.cpp"},
        )

    def test_returns_empty_set_when_nothing_changed(self) -> None:
        self.assertEqual(changed_paths(self.base, cwd=self.repo), set())

    def test_compares_against_merge_base_not_branch_tip(self) -> None:
        git(self.repo, "checkout", "-q", "-b", "feature")
        (self.repo / "src" / "feature.cpp").write_text("int feature() { return 0; }\n")
        commit_all(self.repo, "feature work")

        git(self.repo, "checkout", "-q", "-")
        (self.repo / "src" / "mainline.cpp").write_text("int mainline() { return 0; }\n")
        commit_all(self.repo, "mainline work")
        mainline_tip = git(self.repo, "rev-parse", "HEAD")

        git(self.repo, "checkout", "-q", "feature")

        self.assertEqual(
            changed_paths(mainline_tip, cwd=self.repo),
            {self.repo / "src" / "feature.cpp"},
        )

    def test_unknown_ref_exits_with_error(self) -> None:
        with self.assertRaises(typer.Exit):
            changed_paths("does-not-exist", cwd=self.repo)


class TestOnlyChanged(unittest.TestCase):
    def test_keeps_only_files_present_in_changed_set(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            changed_file = base / "changed.cpp"
            untouched_file = base / "untouched.cpp"
            changed_file.write_text("")
            untouched_file.write_text("")

            result = only_changed([changed_file, untouched_file], {changed_file})

            self.assertEqual(result, [changed_file])

    def test_ignores_changed_files_outside_the_given_files(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            base = Path(tmpdir).resolve()
            in_scope = base / "in_scope.cpp"
            in_scope.write_text("")

            result = only_changed([in_scope], {base / "out_of_scope.cpp"})

            self.assertEqual(result, [])


if __name__ == "__main__":
    unittest.main()
