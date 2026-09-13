from pathlib import Path

import typer

from karkinolution import style, tidy

app = typer.Typer(help="Development tools for the Karkinolution project.")

style_app = typer.Typer(help="Format and check source code using clang-format.")

tidy_app = typer.Typer(help="Analyze and fix source code using clang-tidy.")

app.add_typer(style_app, name="style")
app.add_typer(tidy_app, name="tidy")


@style_app.command(
    "check",
    help="Check whether source files are correctly formatted.",
)
def style_check(paths: list[Path]) -> None:
    style.check(tuple(paths))


@style_app.command(
    "format",
    help="Format source files in-place.",
)
def style_format(paths: list[Path]) -> None:
    style.format(tuple(paths))


@tidy_app.command(
    "check",
    help="Run clang-tidy checks on source files.",
)
def tidy_check(paths: list[Path]) -> None:
    tidy.check(tuple(paths))


@tidy_app.command(
    "fix",
    help="Run clang-tidy and apply available fixes.",
)
def tidy_fix(paths: list[Path]) -> None:
    tidy.fix(tuple(paths))


if __name__ == "__main__":
    app()
