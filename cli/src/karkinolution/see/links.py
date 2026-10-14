from collections.abc import Mapping
from dataclasses import dataclass
from pathlib import Path
from urllib.parse import quote

import typer

FILE_EDITOR = "file"
NO_LINKS = "none"

EDITOR_URL_TEMPLATES = {
    "vscode": "vscode://file{path}:{line}",
    "vscode-insiders": "vscode-insiders://file{path}:{line}",
    "vscodium": "vscodium://file{path}:{line}",
}

SUPPORTED_EDITORS = (FILE_EDITOR, NO_LINKS, *EDITOR_URL_TEMPLATES)


def detect_editor(environ: Mapping[str, str]) -> str:
    if environ.get("TERM_PROGRAM") == "vscode":
        return "vscode"

    return FILE_EDITOR


def resolve_editor(requested: str | None, environ: Mapping[str, str]) -> str:
    if requested is None:
        return detect_editor(environ)

    if requested not in SUPPORTED_EDITORS:
        typer.echo(
            f"Unknown editor '{requested}'. Supported editors: {', '.join(SUPPORTED_EDITORS)}",
            err=True,
        )
        raise typer.Exit(code=1)

    return requested


@dataclass(frozen=True)
class Linker:
    editor: str

    def url(self, path: Path, line: int = 1) -> str | None:
        if self.editor == NO_LINKS:
            return None

        if self.editor == FILE_EDITOR:
            return path.as_uri()

        return EDITOR_URL_TEMPLATES[self.editor].format(path=quote(str(path)), line=line)
