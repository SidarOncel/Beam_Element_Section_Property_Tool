#pragma once

#include <vector>
#include <string>
#include "SectionShape.h"

/*
===============================================================================

Purpose:
Contains helper functions used by multiple geometry classes.

Examples:
- Distance calculations
- Coordinate transformations
- Point generation
- Polygon utilities

Why This File Exists:
Avoids duplicating common geometric calculations throughout the codebase.

Developer Notes:
Only reusable geometry-related functions should be added here.

Do NOT place:
- GUI code
- Property calculations
- FE mesh algorithms

inside this file.

===============================================================================
*/

namespace GeometryUtils
{
    bool isPositive(double value);
    bool isNonNegative(double value);

    // Compare two points using a distance tolerance.
    bool pointEquals(const Point2D& lhs, const Point2D& rhs, double tolerance = 1e-9);

    // Determine whether a polygon is explicitly closed.
    bool isClosedPolygon(const std::vector<Point2D>& points, double tolerance = 1e-9);

    // Validate that a closed polygon has enough vertices and a non-trivial area.
    bool isValidPolygon(const std::vector<Point2D>& points,
                        double minArea = 1e-6,
                        double tolerance = 1e-9);

    // Ensure the polygon is closed, removing duplicate consecutive vertices.
    std::vector<Point2D> closePolygon(std::vector<Point2D> points,
                                      double tolerance = 1e-9);

    // Normalize a polygon by closing it and enforcing the requested winding.
    std::vector<Point2D> normalizePolygon(std::vector<Point2D> points,
                                          bool clockwise = true,
                                          double tolerance = 1e-9);

    double signedArea(const std::vector<Point2D>& points);

    Point2D centroidOfPolygon(const std::vector<Point2D>& points);

    std::string formatError(const std::string& message);
}