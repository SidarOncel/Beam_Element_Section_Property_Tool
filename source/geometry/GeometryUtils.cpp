#include "GeometryUtils.h"

#include <algorithm>
#include <cmath>
#include <sstream>

/*
===============================================================================
Purpose:
Implements reusable helper functions used by the geometry module.

Developer notes:
These functions are intentionally generic. They should support multiple section
types without knowing anything about GUI or property calculations.

===============================================================================
*/

namespace GeometryUtils
{
    bool isPositive(double value)
    {
        return value > 0.0;
    }

    bool isNonNegative(double value)
    {
        return value >= 0.0;
    }

    // Compare two 2D points with a tolerance to handle floating point imprecision.
    bool pointEquals(const Point2D& lhs, const Point2D& rhs, double tolerance)
    {
        const double dy = lhs.y - rhs.y;
        const double dz = lhs.z - rhs.z;
        return (dy * dy + dz * dz) <= (tolerance * tolerance);
    }

    // Determine whether the polygon is explicitly closed by comparing first and last points.
    bool isClosedPolygon(const std::vector<Point2D>& points, double tolerance)
    {
        if (points.size() < 3)
            return false;

        return pointEquals(points.front(), points.back(), tolerance);
    }

    // Validate that the polygon is closed and has a minimum non-zero area.
    bool isValidPolygon(const std::vector<Point2D>& points,
                        double minArea,
                        double tolerance)
    {
        if (points.size() < 4) // closed polygons must have at least 4 points (first == last)
            return false;

        if (!isClosedPolygon(points, tolerance))
            return false;

        const double area = std::abs(signedArea(points));
        return area >= minArea;
    }

    // Close a polygon by removing duplicate consecutive vertices and ensuring the
    // first and last points match within the provided tolerance.
    std::vector<Point2D> closePolygon(std::vector<Point2D> points, double tolerance)
    {
        if (points.empty())
            return points;

        std::vector<Point2D> trimmed;
        trimmed.reserve(points.size());

        Point2D previous = points.front();
        trimmed.push_back(previous);

        for (std::size_t i = 1; i < points.size(); ++i)
        {
            if (!pointEquals(previous, points[i], tolerance))
            {
                trimmed.push_back(points[i]);
                previous = points[i];
            }
        }

        if (trimmed.size() >= 2 && !pointEquals(trimmed.front(), trimmed.back(), tolerance))
        {
            trimmed.push_back(trimmed.front());
        }

        return trimmed;
    }

    // Normalize polygon closure and orientation.
    // The returned polygon is closed and oriented according to the 'clockwise' flag.
    std::vector<Point2D> normalizePolygon(std::vector<Point2D> points,
                                          bool clockwise,
                                          double tolerance)
    {
        points = closePolygon(std::move(points), tolerance);

        if (points.size() < 4)
            return points;

        const double area = signedArea(points);
        if (std::abs(area) < tolerance)
            return points;

        const bool isClockwise = (area < 0.0);
        if (clockwise != isClockwise)
        {
            // Reverse orientation while preserving closure.
            points.pop_back();
            std::reverse(points.begin(), points.end());
            points.push_back(points.front());
        }

        return points;
    }

    double signedArea(const std::vector<Point2D>& points)
    {
        if (points.size() < 3)
            return 0.0;

        double area2 = 0.0;
        for (size_t i = 0; i < points.size(); ++i)
        {
            const Point2D& p1 = points[i];
            const Point2D& p2 = points[(i + 1) % points.size()];
            area2 += (p1.y * p2.z) - (p2.y * p1.z);
        }

        return 0.5 * area2;
    }

    Point2D centroidOfPolygon(const std::vector<Point2D>& points)
    {
        Point2D centroid{0.0, 0.0};

        if (points.size() < 3)
            return centroid;

        const double A = signedArea(points);
        if (std::abs(A) < 1e-12)
            return centroid;

        double cy = 0.0;
        double cz = 0.0;

        for (size_t i = 0; i < points.size(); ++i)
        {
            const Point2D& p1 = points[i];
            const Point2D& p2 = points[(i + 1) % points.size()];
            const double cross = (p1.y * p2.z) - (p2.y * p1.z);

            cy += (p1.y + p2.y) * cross;
            cz += (p1.z + p2.z) * cross;
        }

        centroid.y = cy / (6.0 * A);
        centroid.z = cz / (6.0 * A);
        return centroid;
    }

    std::string formatError(const std::string& message)
    {
        std::ostringstream oss;
        oss << "Geometry error: " << message;
        return oss.str();
    }
}