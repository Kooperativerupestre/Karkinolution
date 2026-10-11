import json
from pathlib import Path
from typing import Any

import typer

from karkinolution.paths import find_config_file

PRESETS_FILE = "CMakePresets.json"

NOT_INHERITED_KEYS = {"name", "hidden", "inherits", "displayName"}

Preset = dict[str, Any]


def find_repo_root(start: Path | None = None) -> Path:
    presets_file = find_config_file(PRESETS_FILE, start)

    if presets_file is None:
        typer.echo(
            f"{PRESETS_FILE} not found. Run this command inside the Karkinolution repository.",
            err=True,
        )
        raise typer.Exit(code=1)

    return presets_file.parent


def load_configure_presets(root: Path) -> dict[str, Preset]:
    data = json.loads((root / PRESETS_FILE).read_text(encoding="utf-8"))
    return {preset["name"]: preset for preset in data.get("configurePresets", [])}


def merge_preset(target: Preset, source: Preset) -> None:
    for key, value in source.items():
        if key in NOT_INHERITED_KEYS:
            continue

        if key == "cacheVariables":
            target.setdefault("cacheVariables", {}).update(value)
        else:
            target[key] = value


def resolve_inheritance(name: str, presets: dict[str, Preset]) -> Preset:
    preset = presets[name]
    parents = preset.get("inherits", [])

    if isinstance(parents, str):
        parents = [parents]

    resolved: Preset = {}

    # CMake prefers earlier parents, so the later ones are merged first.
    for parent in reversed(parents):
        merge_preset(resolved, resolve_inheritance(parent, presets))

    merge_preset(resolved, preset)
    return resolved


def resolve_preset(name: str, presets: dict[str, Preset]) -> Preset:
    if name not in presets or presets[name].get("hidden", False):
        available = ", ".join(sorted(n for n, p in presets.items() if not p.get("hidden", False)))
        typer.echo(f"Unknown preset '{name}'. Available presets: {available}", err=True)
        raise typer.Exit(code=1)

    return resolve_inheritance(name, presets)


def cache_value(resolved: Preset, variable: str) -> str | None:
    value = resolved.get("cacheVariables", {}).get(variable)

    if isinstance(value, dict):
        value = value.get("value")

    if isinstance(value, bool):
        return "ON" if value else "OFF"

    return None if value is None else str(value)


def binary_dir(name: str, resolved: Preset, root: Path) -> Path:
    template: str = resolved["binaryDir"]
    return Path(template.replace("${sourceDir}", str(root)).replace("${presetName}", name))
