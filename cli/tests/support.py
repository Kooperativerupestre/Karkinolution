import json
from pathlib import Path

MINIMAL_PRESETS = {
    "version": 6,
    "configurePresets": [
        {"name": "base", "hidden": True, "generator": "Ninja"},
        {
            "name": "clang",
            "hidden": True,
            "cacheVariables": {"CMAKE_C_COMPILER": "clang", "CMAKE_CXX_COMPILER": "clang++"},
        },
        {
            "name": "debug",
            "inherits": ["base", "clang"],
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {"CMAKE_BUILD_TYPE": "Debug"},
        },
    ],
}


def write_minimal_repo(root: Path) -> None:
    (root / "CMakePresets.json").write_text(json.dumps(MINIMAL_PRESETS), encoding="utf-8")
