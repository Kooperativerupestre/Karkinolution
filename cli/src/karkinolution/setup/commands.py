import subprocess

import typer

from karkinolution.paths import complete
from karkinolution.presets import find_repo_root, load_configure_presets, resolve_preset
from karkinolution.setup.checks import Problem, run_checks


def report(problems: list[Problem]) -> None:
    for problem in problems:
        typer.echo(f"Problem: {problem.message}\n  Fix: {problem.fix}", err=True)


def configure(preset: str = "debug") -> None:
    root = find_repo_root()
    resolved = resolve_preset(preset, load_configure_presets(root))

    problems = run_checks(root, resolved)
    if problems:
        report(problems)
        raise typer.Exit(code=1)

    typer.echo(f"Environment OK. Configuring preset '{preset}'.")

    result = subprocess.run(["cmake", "--preset", preset], cwd=root, check=False)
    if result.returncode != 0:
        raise typer.Exit(code=result.returncode)

    complete()
