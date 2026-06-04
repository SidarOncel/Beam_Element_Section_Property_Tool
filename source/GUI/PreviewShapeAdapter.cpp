#include "PreviewShapeAdapter.hpp"

#include <cmath>

namespace
{
    constexpr double kPi = 3.14159265358979323846;

    void closeBoundary(std::vector<Point2D>& points)
    {
        if (!points.empty())
        {
            points.push_back(points.front());
        }
    }

    std::vector<Point2D> rectangle(double minY, double minZ, double maxY, double maxZ)
    {
        std::vector<Point2D> points{
            {minY, minZ},
            {maxY, minZ},
            {maxY, maxZ},
            {minY, maxZ}
        };
        closeBoundary(points);
        return points;
    }

    std::vector<Point2D> circle(double radius, int segments)
    {
        std::vector<Point2D> points;
        points.reserve(static_cast<std::size_t>(segments + 1));

        for (int i = 0; i < segments; ++i)
        {
            const double angle = (2.0 * kPi * static_cast<double>(i)) /
                                 static_cast<double>(segments);
            points.push_back({radius * std::cos(angle), radius * std::sin(angle)});
        }

        closeBoundary(points);
        return points;
    }

    SectionShape hSectionShape(const SectionInput& input)
    {
        const double halfWidth = input.width / 2.0;
        const double halfHeight = input.height / 2.0;
        const double halfWeb = input.webThickness / 2.0;
        const double topWebZ = halfHeight - input.flangeThickness;
        const double bottomWebZ = -halfHeight + input.flangeThickness;

        SectionShape shape;
        shape.boundaryPoints = {
            {-halfWidth, halfHeight},
            {halfWidth, halfHeight},
            {halfWidth, topWebZ},
            {halfWeb, topWebZ},
            {halfWeb, bottomWebZ},
            {halfWidth, bottomWebZ},
            {halfWidth, -halfHeight},
            {-halfWidth, -halfHeight},
            {-halfWidth, bottomWebZ},
            {-halfWeb, bottomWebZ},
            {-halfWeb, topWebZ},
            {-halfWidth, topWebZ}
        };
        closeBoundary(shape.boundaryPoints);
        shape.isClosed = true;
        return shape;
    }

    SectionShape boxSectionShape(const SectionInput& input)
    {
        const double top = input.topFlangeThickness > 0.0 ? input.topFlangeThickness
                                                          : input.wallThickness;
        const double bottom = input.bottomFlangeThickness > 0.0 ? input.bottomFlangeThickness
                                                                : input.wallThickness;
        const double left = input.leftWebThickness > 0.0 ? input.leftWebThickness
                                                         : input.wallThickness;
        const double right = input.rightWebThickness > 0.0 ? input.rightWebThickness
                                                           : input.wallThickness;

        const double halfWidth = input.width / 2.0;
        const double halfHeight = input.height / 2.0;

        SectionShape shape;
        shape.boundaryPoints = rectangle(-halfWidth, -halfHeight, halfWidth, halfHeight);
        shape.holes.push_back(rectangle(-halfWidth + left,
                                        -halfHeight + bottom,
                                        halfWidth - right,
                                        halfHeight - top));
        shape.isClosed = true;
        return shape;
    }

    SectionShape pipeSectionShape(const SectionInput& input)
    {
        SectionShape shape;
        shape.boundaryPoints = circle(input.diameter / 2.0, 96);
        shape.holes.push_back(circle(input.diameter / 2.0 - input.wallThickness, 96));
        shape.isClosed = true;
        return shape;
    }

    SectionShape girderSectionShape(const SectionInput& input)
    {
        const double topLeft = (input.A - input.B) / 2.0;
        const double topRight = topLeft + input.B;

        SectionShape shape;
        shape.boundaryPoints = {
            {0.0, 0.0},
            {input.D, 0.0},
            {input.D, input.f},
            {input.G, input.f},
            {input.A, input.W},
            {input.A, input.H},
            {topRight, input.H},
            {topRight, input.H + input.e},
            {topLeft, input.H + input.e},
            {topLeft, input.H},
            {0.0, input.H}
        };
        closeBoundary(shape.boundaryPoints);
        shape.isClosed = true;
        return shape;
    }
}

SectionShape PreviewShapeAdapter::createShape(const SectionInput& input)
{
    switch (input.type)
    {
    case SectionType::HSection:
        return hSectionShape(input);
    case SectionType::BoxSection:
        return boxSectionShape(input);
    case SectionType::PipeSection:
        return pipeSectionShape(input);
    case SectionType::GirderSection:
        return girderSectionShape(input);
    }

    return {};
}
