# Contributing Guide - Schedule Manager

To maintain a clean, semantic, and readable Git history, this project enforces strict commit message guidelines.

## Commit Message Format

All commit messages must follow this structure:
`type: brief description in lowercase using imperative mood`

*Example:* `add(creature): implement creature brain`

---

## Allowed Types

Use the following types to categorize your commits:

- **add**: Use for any new feature, table, or functionality addition.
  
- **fix**: Use to fix a bug or syntax error, provided it does not alter structural or architectural layers.
  
- **refactor**: Use for structural changes, codebase reorganization, or architectural rewrites without necessarily introducing new features.
  
- **enhances**: Use exclusively when improving or optimizing an existing feature or performance.
  
- **delete**: Use for any removal of functionality, code, or database schemas.

- **docs**: Use for any change of docs.
---

## Commit Guidelines

1. **Use the Imperative Mood**: Always write the commit description as a command or instruction. (e.g., use `implement(theme)`, `fix(theme)`, `add(theme)`, or `delete(theme)`; never use `implemented`, `fixed`, `adds`, or `removed`).
2. **Keep Descriptions Concise**: Keep the commit line under 72 characters. If you need to provide more context, add a blank line below it followed by a detailed body.