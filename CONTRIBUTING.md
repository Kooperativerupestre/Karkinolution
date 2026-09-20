# Contributing Guide - Karkinolution

To maintain a clean, semantic, and readable Git history, this project enforces strict commit message guidelines.

## Commit Message Format

All commit messages must follow this structure:

`type(scope): brief description in lowercase using imperative mood`

*Example:* `add(creature): implement creature brain`

---

## Allowed Types

Use the following types to categorize your commits:

* **add**: Use for any new feature, table, or functionality addition.

* **fix**: Use to correct incorrect or unintended behavior, including bugs and syntax errors.

* **refactor**: Use for structural changes, codebase reorganization, or architectural rewrites without changing
  externally observable behavior.

* **enhance**: Use exclusively when improving an existing feature or functionality without fundamentally changing its
  purpose.

* **delete**: Use for any removal of functionality, code, or database schemas.

* **docs**: Use for any change to documentation.

* **merge**: Use when integrating changes from one branch into another.

* **build**: Use for changes to the build system, build configuration, compiler configuration, or build dependencies.

* **style**: Use for changes that affect code style or formatting without changing functionality.

---

## Commit Guidelines

1. **Use the Imperative Mood**: Always write the commit description as a command or instruction. (e.g., use
   `implement(theme)`, `fix(theme)`, `add(theme)`, or `delete(theme)`; never use `implemented`, `fixed`, `adds`, or
   `removed`).

2. **Keep Descriptions Concise**: The description should clearly communicate the purpose of the commit without
   unnecessary detail.

3. **Use the Appropriate Type**: Choose the type that most accurately describes the purpose of the commit. Do not use a
   generic type when a more specific one applies.

4. **Use a Meaningful Scope**: The scope should identify the component, module, or area affected by the commit.

5. **Keep the Commit Focused**: A commit should represent one logical change whenever possible.
