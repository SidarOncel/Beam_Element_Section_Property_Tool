# Beam Element Section Property Tool

## Overview

Beam Element Section Property Tool is a C++17 and Qt 5.15.2 based engineering application for calculating cross-sectional properties of beam elements used in bridge and structural engineering.

The software supports:

* H Sections
* Box Sections
* Pipe Sections
* Quayside Crane Girder Sections

The tool provides:

* Parametric section creation
* Section property calculation
* Stress point management
* Finite element mesh visualization
* DLL interface for external software integration

---

## Development Environment

| Item            | Requirement     |
| --------------- | --------------- |
| OS              | Windows 10 / 11 |
| Language        | C++17           |
| GUI Framework   | Qt 5.15.2       |
| Build System    | CMake           |
| Version Control | Git + GitHub    |

---

## Project Structure

```text
Beam_Element_Section_Property_Tool/
│
├── Source/
│   ├── API/
│   ├── Core/
│   ├── Geometry/
│   ├── GUI/
│   ├── Mesh/
│   └── Tests/
│
├── Library/
│   ├── bin/
│   ├── lib/
│   └── include/
│
├── Examples/
│   ├── example1/
│   └── example2/
│
├── Documents/
│
├── README.md
├── CMakeLists.txt
└── .gitignore
```

---

## Functional Requirements

### General Tab

* Section selection
* Geometry parameter input
* Section preview
* Automatic property calculation

### Stress Points Tab

* Display default stress points
* Edit stress point coordinates
* Synchronize with section geometry

### FE Mesh Tab

* Mesh size control
* Mesh generation
* Graphical mesh visualization

### Supported Section Types

1. H Section
2. Box Section
3. Pipe Section
4. Quayside Crane Girder Section

---

## Team Responsibilities

### Member 1 — Project Manager & Git Coordinator

Responsibilities:

* Repository management
* Branch management
* Merge requests
* Weekly integration
* Progress tracking
* Final release preparation

Deliverables:

* Git workflow
* Integration reports
* Final project packaging

---

### Member 2 — Geometry Module

What to do:

Implement the mathematical and geometric representation of each section type.
Build the shape definitions for H section, Box section, Pipe section, and Quayside crane girder section.
Define the coordinate system and the parameters used by each section.

Why to do it:

The rest of the software depends on correct geometry.
If the section shape is wrong, all computed properties will also be wrong.
Geometry is the base for property calculation, stress points, and mesh generation.

How to do it:

Study the dimensions shown in the requirement document.
Create classes or structures for each section type.
Store section dimensions in a clear format.
Generate boundary points or centerline data for later use by the property and mesh modules.
Make sure units are consistent, using millimeters as required.

Detailed tasks:

Define input parameters for every section type.
Create geometry classes.
Validate input values so invalid dimensions are rejected.
Produce the shape data needed by the GUI preview and computation engine.

---

### Member 3 — Section Property Calculation Module

What to do:

Implement the formulas for area, centroid, moments of inertia, torsional inertia, and shear areas.
Calculate all section properties required by the assignment.
Return values in a format the GUI and DLL can use.

Why to do it:

This is the core engineering function of the tool.
The output properties are what users need for finite element modeling and structural analysis.
The software loses value if these calculations are inaccurate.

How to do it:

Derive formulas for each supported section type.
Test each formula on simple known shapes first.
Compare results against hand calculations or reference values.
Keep the calculation code separate from the GUI so it can also be used from the DLL.

Detailed tasks:

Compute cross-sectional area.
Compute centroid coordinates.
Compute moments of inertia in the Y and Z directions.
Compute principal axis properties.
Compute torsional properties and shear areas where required.
Add verification cases for each section type.

---

### GUI Development

What to do:

Build the Qt interface with the required tabs: General, Stress Points, and FE Mesh.
Display section drawings, input fields, and property tables.
Connect user input to the backend calculation modules.

Why to do it:

The GUI is how users interact with the software.
A clear interface reduces mistakes in dimension entry.
The assignment explicitly requires a visual interface with multiple functional tabs.

How to do it:

Use Qt 5.15.2 and C++17.
Create a main window and separate tab widgets.
Place the section preview on the right side and input tables on the left side, similar to the sample screenshots in the document.
Update the view when the user changes input values.
Keep the GUI modular so each tab can be maintained separately.

Detailed tasks:

Build the main window layout.
Create the General tab input forms.
Show calculated properties in a table.
Display the current section shape.
Connect the interface to the geometry and calculation modules.
Handle user actions such as changing section type or editing dimensions.

---

### Member 5 — Stress Points and FE Mesh Module

What to do:

Generate the default stress points for each section type.
Allow users to edit stress point coordinates.
Generate and display the FE mesh for the section.

Why to do it:

The document requires stress output points at weld seams and mesh visualization.
These outputs are important for fatigue assessment and finite element analysis.
They also make the software more useful and closer to industrial tools.

How to do it:

Define the default stress point locations from the requirement document.
Make the stress points editable in a tabular form.
Build a mesh generator that can divide the section into small elements.
Update the graphical display whenever the mesh size changes.

Detailed tasks:

Create a stress point model.
Store the coordinates of points 1, 2, 3, and 4 for each section.
Allow manual editing of those points.
Implement a mesh density control.
Draw the mesh in the FE Mesh tab.
Keep mesh generation independent from the GUI so it can also support the DLL later.

---

### Member 6 — DLL/API, Examples, Testing, and Documentation

What to do:

Expose the core functions through a DLL interface.
Write example programs showing how to call the DLL.
Prepare the user manual, test report, and supporting documents.

Why to do it:

The assignment requires a self-compilable DLL.
Example programs prove that the DLL works correctly.
Documentation is required for submission and grading.

How to do it:

Put the exported functions in a clean API layer.
Keep the interface simple and stable for external programs.
Write at least two example projects that show typical usage.
Document how to build, run, and use the project.
Write test cases with expected and actual outputs.

Detailed tasks:

Create exported DLL functions such as property calculation and mesh creation.
Build example applications in the Examples/ folder.
Write the user manual with installation and usage steps.
Prepare test cases for each section type.
Record comparison results against expected values.
Organize the final documentation folder.

---

## Git Workflow

Main branches:

* main
* develop

Feature branches:

* feature-geometry
* feature-properties
* feature-gui
* feature-mesh
* feature-api
* feature-docs

---

## Deliverables

* Dynamic Link Library (DLL)
* Source Code
* Example Programs
* User Manual
* Architecture Design Document
* Test Report
* Presentation Slides
* Demonstration Video
