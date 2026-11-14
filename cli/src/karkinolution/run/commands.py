import os
import subprocess

import typer

from karkinolution.presets import binary_dir, find_repo_root, load_configure_presets, resolve_preset

CORE_TARGET = "karkinolution_core"
APP_TARGET = "karkinolution_app"


def launch(preset: str = "debug", target: str | None = None) -> None:
    root = find_repo_root()
    resolved = resolve_preset(preset, load_configure_presets(root))
    build_dir = binary_dir(preset, resolved, root)

    if not (build_dir / "CMakeCache.txt").is_file():
        typer.echo(
            f"Build directory '{build_dir}' is not configured. Run: karkinolution setup --preset {preset}",
            err=True,
        )
        raise typer.Exit(code=1)

    selected_target = target or (APP_TARGET if "app" in preset else CORE_TARGET)
    targets = [selected_target]

    cmd = ["cmake", "--build", str(build_dir)]
    for t in targets:
        cmd.extend(["--target", t])

    build = subprocess.run(
        cmd,
        check=False,
    )
    if build.returncode != 0:
        raise typer.Exit(code=build.returncode)

    executable = str(build_dir / selected_target)

    os.chdir(root)
    os.execv(executable, [executable])
