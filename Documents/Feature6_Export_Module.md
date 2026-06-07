# Feature 6 Export Module

## Scope of the first slice

This first implementation slice adds a pure C++ export formatting module under
`source/Core`.

The goal is to create reusable string-formatting functions for:

* human-readable section property reports
* FEM-card-style plain text export

This slice intentionally does **not** include:

* Qt or GUI integration
* API/DLL integration
* file dialogs
* JSON or YAML serialization
* full save/load support for project files

## Existing types reused

The module reuses the existing shared repository types:

* `SectionType` from `source/geometry/SectionTypes.hpp`
* `SectionInput` from `source/geometry/SectionInput.hpp`
* `SectionProperties` from `source/Core/PropertyCalculator.hpp`

## Output rules

The formatter uses the following rules:

* default precision is two decimal places
* units are fixed to `mm`, `mm^2`, and `mm^4`
* field ordering is stable
* only input fields relevant to the active section type are exported
* section names follow the existing `SectionType` display strings

## Why this is in `source/Core`

Feature 6 needs to stay reusable by:

* future GUI export actions
* future API/DLL bindings
* examples
* tests

Placing the formatter in the core layer avoids any dependency on Qt or
`SectionState`.

## Future extensions

Likely next steps after this slice:

1. add file-writing helpers for report export
2. add project parameter save/load support
3. add sample exports for all section types
4. expose export functions through the API layer once that layer is stabilized
5. integrate export actions into the GUI
