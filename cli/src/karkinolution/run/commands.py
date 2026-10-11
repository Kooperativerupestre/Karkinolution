import os
import subprocess

import typer

from karkinolution.presets import binary_dir, find_repo_root, load_configure_presets, resolve_preset

APP_TARGET = "karkinolution_app"


def launch(preset: str = "debug") -> None:
    root = find_repo_root()
    build_dir = binary_dir(preset, resolve_preset(preset, load_configure_presets(root)), root)

    if not (build_dir / "CMakeCache.txt").is_file():
        typer.echo(
            f"Build directory '{build_dir}' is not configured. Run: karkinolution setup --preset {preset}",
            err=True,
        )
        raise typer.Exit(code=1)

    build = subprocess.run(
        ["cmake", "--build", str(build_dir), "--target", APP_TARGET],
        check=False,
    )
    if build.returncode != 0:
        raise typer.Exit(code=build.returncode)

    executable = str(build_dir / APP_TARGET)

    os.chdir(root)
    os.execv(executable, [executable])
