#pragma once

#include <vector>

/*
===============================================================================
File: SectionShape.h

Purpose:
Defines the geometric output produced by the Geometry Module.

The generated shape is later used by:
- Property calculations
- Stress point generation
- FE mesh generation
- Section visualization

Coordinate Convention:
Y-axis:
    Horizontal

Z-axis:
    Vertical

Units:
    Millimeters (mm)

Developer Notes:
Boundary points should be ordered consistently, preferably clockwise.

Future Extensions:
Additional geometry information such as holes, centerlines, plate
segments, or curved boundaries may be added here.

===============================================================================
*/

struct Point2D
{
    double y = 0.0;
    double z = 0.0;
};

struct SectionShape
{
    std::vector<Point2D> boundaryPoints;

    // Hollow sections may contain one or more inner boundaries.
    // Each inner boundary is represented as a closed polygon of points.
    std::vector<std::vector<Point2D>> holes;

    // Indicates whether the returned boundary points are already closed.
    bool isClosed = false;
};