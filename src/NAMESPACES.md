# Project Structure

## About

This namespace organization is relatively new as of October 10, 2026.

Since the project already has a substantial codebase, some namespaces have not yet been reorganized, and some are not documented in this tree.

## Want to Contribute?

Contributions to this reorganization are welcome! You can help by organizing existing files into appropriate namespaces and keeping the project structure consistent.

## Ongoing Reorganization

I'm gradually organizing the codebase by placing new files in appropriate namespaces and updating existing files to follow the new organization.

## Reliability

**This tree is a work in progress.** It may be incomplete or outdated, so do not assume it accurately represents the entire project.

## Tree

```text
Frames/
├── Types
├── FieldsBytes
├── Offsets
├── Motor
├── Parser
├── Processor
└── Models

Karkinolution/
├── Binary/
│   └── Interpreters/
│       ├── SRI/
│       │   ├── Creature/
│       │   ├── │   ├── BytesArrays
│       │   └── │   └── Layout
│       │   ├── Corpse/
│       │   │   ├── BytesArrays
│       │   │   └── Layout
│       │   ├── Soil/
│       │   │   ├── BytesArrays
│       │   │   └── Layout
│       │   └── Territory/
│       │       ├── BytesArrays
│       │       └── Layout
│       └── DSI/
│           ├── Creature
│           ├── Corpse
│           ├── Soil
│           └── Territory
└── Utils/
    └── EnumsToString

KarkinolutionGodot/
└── Models/
    ├── Organisms/
    │   ├── Corpse
    │   └── Creature
    ├── Properties
    └── Math/
        ├── Stats
        └── Units
```