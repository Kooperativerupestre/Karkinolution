from pathlib import Path

from karkinolution import style, tidy


def check(
    paths: tuple[Path, ...],
    build_dir: Path | None = None,
    warnings_as_errors: bool = True,
    style_file: Path | None = None,
    config_file: Path | None = None,
    changed_since: str | None = None,
) -> None:
    style.check(paths, style_file=style_file)
    tidy.check(
        paths,
        build_dir=build_dir,
        warnings_as_errors=warnings_as_errors,
        config_file=config_file,
        changed_since=changed_since,
    )


def fix(
    paths: tuple[Path, ...],
    build_dir: Path | None = None,
    style_file: Path | None = None,
    config_file: Path | None = None,
) -> None:
    style.format(paths, style_file=style_file)
    tidy.fix(paths, build_dir=build_dir, config_file=config_file)
