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
    double y;
    double z;
};

struct SectionShape
{
    std::vector<Point2D> boundaryPoints;
};