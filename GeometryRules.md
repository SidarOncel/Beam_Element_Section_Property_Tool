# Geometry Module Design Rules

## Introduction

The Geometry Module is the foundation of the Beam Element Section Property Tool. Before the software can calculate section properties, generate stress points, create finite element meshes, or display a section in the graphical user interface, it must first understand the exact shape of the cross-section.

The purpose of this document is to define how cross-sectional geometry will be represented inside the software so that every team member follows the same conventions. By establishing these rules early, we can avoid inconsistencies between different modules and simplify future integration.

---

## Role of the Geometry Module

The Geometry Module is responsible for transforming user-defined dimensions into a mathematical representation of the cross-section.

For example, when a user enters the dimensions of an H-section:

* Overall height
* Flange width
* Flange thickness
* Web thickness

the geometry module will convert these values into points, lines, and boundaries that can later be used by:

* The Property Calculation Module to compute area and moments of inertia.
* The Stress Point Module to locate weld inspection points.
* The FE Mesh Module to generate finite element meshes.
* The GUI Module to display the section visually.

Therefore, geometry generation is the first major step in the software workflow.

---

## Coordinate System

All sections shall be created in a two-dimensional Y-Z coordinate system.

The project specification defines the cross-sectional plane using the Y and Z axes. To ensure consistency across all modules, the following convention will be adopted:

* Positive Y direction points to the right.
* Positive Z direction points upward.
* The origin (0,0) is located at the geometric center of the section whenever possible.

Using a common origin simplifies many engineering calculations, particularly centroid calculations and moment of inertia computations.

Example:

```text id="ujj1eq"
          +Z
           ↑
           |
           |
-----------+----------→ +Y
           |
           |
```

---

## Units

All geometric dimensions shall be stored internally in millimeters (mm).

Examples:

* Width = 500 mm
* Height = 1200 mm
* Thickness = 16 mm

No unit conversions will be performed inside the Geometry Module.

The responsibility of the Geometry Module is only to represent geometry correctly. Any unit conversion requirements in future versions should be handled separately.

---

## Supported Section Types

The first version of the software supports four cross-section types:

### H Section

Commonly used in steel structures and bridge engineering.

Input dimensions include:

* Overall height
* Flange width
* Flange thickness
* Web thickness

### Box Section

A closed hollow section composed of plates.

Input dimensions include:

* Width
* Height
* Wall thickness

### Pipe Section

A circular hollow section.

Input dimensions include:

* Outer diameter
* Wall thickness

### Quayside Crane Girder Section

A more complex engineering section composed of multiple plates.

Its geometry will be generated according to the dimensions provided in the project requirements document.

---

## Geometry Representation Strategy

The software must be able to display sections, generate meshes, and perform engineering calculations.

To support all these functions, every section will eventually be represented as a collection of boundary points.

For example, instead of storing only:

```text id="hh6jzy"
Width = 300 mm
Height = 500 mm
```

the software will generate the actual shape using coordinate points:

```text id="dgvnfr"
Point 1 = (-150, -250)
Point 2 = (150, -250)
Point 3 = (150, 250)
Point 4 = (-150, 250)
```

This representation allows other modules to work directly with the geometry regardless of section type.

---

## Validation Rules

Before generating any geometry, the software must verify that the user input is physically reasonable.

Examples of invalid input:

* Negative dimensions
* Zero thickness
* Thickness larger than section width
* Thickness larger than section height

If invalid values are detected, the Geometry Module shall reject the geometry and provide an appropriate error message.

Example:

```text id="h4r1e4"
Error:
Flange thickness cannot exceed section height.
```

Input validation should occur before any calculations are performed.

---

The current implementation focuses on four predefined section types.
However, the geometry framework should be designed so that additional section types can be added later without major modifications.