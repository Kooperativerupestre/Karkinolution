import re
import shutil
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path

from karkinolution.presets import Preset, cache_value

GENERATOR_TOOLS = {"Ninja": "ninja"}

SUBMODULE_PATH_PATTERN = re.compile(r"^\s*path\s*=\s*(\S+)\s*$", re.MULTILINE)


@dataclass(frozen=True)
class Problem:
    message: str
    fix: str


def required_tools(resolved: Preset) -> list[str]:
    tools = ["cmake"]

    generator_tool = GENERATOR_TOOLS.get(resolved.get("generator", ""))
    if generator_tool:
        tools.append(generator_tool)

    for variable in ("CMAKE_C_COMPILER", "CMAKE_CXX_COMPILER"):
        compiler = cache_value(resolved, variable)
        if compiler:
            tools.append(compiler)

    return tools


def missing_tool_problems(
    tools: list[str],
    which: Callable[[str], str | None] = shutil.which,
) -> list[Problem]:
    return [
        Problem(
            message=f"'{tool}' was not found on PATH.",
            fix=f"Install {tool} and make sure it is on PATH.",
        )
        for tool in tools
        if which(tool) is None
    ]


def declared_submodules(root: Path) -> list[str]:
    gitmodules = root / ".gitmodules"

    if not gitmodules.is_file():
        return []

    return SUBMODULE_PATH_PATTERN.findall(gitmodules.read_text(encoding="utf-8"))


def required_submodules(root: Path, resolved: Preset) -> list[str]:
    submodules = declared_submodules(root)
    if cache_value(resolved, "KARKINOLUTION_BUILD_VIEW") != "ON":
        return [s for s in submodules if not s.endswith("raylib")]
    return submodules


def is_populated(path: Path) -> bool:
    return path.is_dir() and any(path.iterdir())


def submodule_problems(root: Path, submodules: list[str]) -> list[Problem]:
    missing = [submodule for submodule in submodules if not is_populated(root / submodule)]

    if not missing:
        return []

    return [
        Problem(
            message=f"Submodules not initialized: {', '.join(missing)}",
            fix=f"git submodule update --init {' '.join(missing)}",
        )
    ]


def run_checks(
    root: Path,
    resolved: Preset,
    which: Callable[[str], str | None] = shutil.which,
) -> list[Problem]:
    return [
        *missing_tool_problems(required_tools(resolved), which),
        *submodule_problems(root, required_submodules(root, resolved)),
    ]
