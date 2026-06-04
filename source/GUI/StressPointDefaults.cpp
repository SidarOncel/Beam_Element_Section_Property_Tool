#include "StressPointDefaults.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    // Reference point coordinates come from Documents/Test data.pdf.
    constexpr double kBoxReferenceWidth = 1320.0;
    constexpr double kBoxReferenceHeight = 2600.0;
    constexpr double kBoxReferenceHalfY = 641.0;
    constexpr double kBoxReferenceBottomZ = -1491.6;
    constexpr double kBoxReferenceTopZ = 1108.4;
    constexpr double kBoxReferenceCentroidFromBottom = 1491.6;

    constexpr double kGirderReferenceWidth = 1160.0;
    constexpr double kGirderReferenceHeight = 2020.0;
    constexpr double kGirderReferenceCentroidY = 543.2;
    constexpr double kGirderReferenceCentroidZ = 941.2;
    constexpr double kGirderReferencePrincipalAngle = 2.1933e-01;

    StressPoint makePoint(int id, double y, double z)
    {
        return {id, {y, z}};
    }

    double scaleBy(double value, double reference)
    {
        return reference > 0.0 ? value / reference : 0.0;
    }

    double girderWidth(const SectionInput& input)
    {
        return std::max(std::max(input.A, input.B), std::max(input.G, input.D));
    }

    double girderHeight(const SectionInput& input)
    {
        return input.H + input.e;
    }

    std::vector<StressPoint> hSectionStressPoints(const SectionInput& input)
    {
        const double halfWeb = input.webThickness / 2.0;
        const double jointZ = input.height / 2.0 - input.flangeThickness;

        return {
            makePoint(1, -halfWeb, -jointZ),
            makePoint(2, halfWeb, -jointZ),
            makePoint(3, halfWeb, jointZ),
            makePoint(4, -halfWeb, jointZ)
        };
    }

    std::vector<StressPoint> boxSectionStressPoints(const SectionInput& input)
    {
        const double y = kBoxReferenceHalfY * scaleBy(input.width, kBoxReferenceWidth);
        const double bottomZ = kBoxReferenceBottomZ * scaleBy(input.height, kBoxReferenceHeight);
        const double topZ = kBoxReferenceTopZ * scaleBy(input.height, kBoxReferenceHeight);

        return {
            makePoint(1, -y, bottomZ),
            makePoint(2, y, bottomZ),
            makePoint(3, y, topZ),
            makePoint(4, -y, topZ)
        };
    }

    std::vector<StressPoint> pipeSectionStressPoints(const SectionInput& input)
    {
        const double radius = input.diameter / 2.0;

        return {
            makePoint(1, radius, 0.0),
            makePoint(2, 0.0, radius),
            makePoint(3, -radius, 0.0),
            makePoint(4, 0.0, -radius)
        };
    }

    std::vector<StressPoint> girderSectionStressPoints(const SectionInput& input)
    {
        const double widthScale = scaleBy(girderWidth(input), kGirderReferenceWidth);
        const double heightScale = scaleBy(girderHeight(input), kGirderReferenceHeight);

        return {
            makePoint(1, -735.0 * widthScale, -800.5 * heightScale),
            makePoint(2, 328.9 * widthScale, -1037.6 * heightScale),
            makePoint(3, 447.8 * widthScale, 985.0 * heightScale),
            makePoint(4, -299.9 * widthScale, 1151.6 * heightScale)
        };
    }

    std::vector<StressPoint> boxPreviewPoints(const SectionInput& input,
                                              const std::vector<StressPoint>& points)
    {
        std::vector<StressPoint> previewPoints = points;
        const double heightScale = scaleBy(input.height, kBoxReferenceHeight);
        const double centroidOffset = kBoxReferenceCentroidFromBottom * heightScale -
                                      input.height / 2.0;

        for (StressPoint& point : previewPoints)
        {
            point.coordinates.z += centroidOffset;
        }

        return previewPoints;
    }

    std::vector<StressPoint> girderPreviewPoints(const SectionInput& input,
                                                 const std::vector<StressPoint>& points)
    {
        std::vector<StressPoint> previewPoints = points;
        const double widthScale = scaleBy(girderWidth(input), kGirderReferenceWidth);
        const double heightScale = scaleBy(girderHeight(input), kGirderReferenceHeight);
        const double centroidY = kGirderReferenceCentroidY * widthScale;
        const double centroidZ = kGirderReferenceCentroidZ * heightScale;
        const double cosTheta = std::cos(kGirderReferencePrincipalAngle);
        const double sinTheta = std::sin(kGirderReferencePrincipalAngle);

        for (StressPoint& point : previewPoints)
        {
            const double yo = point.coordinates.y;
            const double zo = point.coordinates.z;
            point.coordinates.y = centroidY + yo * cosTheta - zo * sinTheta;
            point.coordinates.z = centroidZ + yo * sinTheta + zo * cosTheta;
        }

        return previewPoints;
    }
}

std::vector<StressPoint> StressPointDefaults::defaultStressPoints(const SectionInput& input)
{
    switch (input.type)
    {
    case SectionType::HSection:
        return hSectionStressPoints(input);
    case SectionType::BoxSection:
        return boxSectionStressPoints(input);
    case SectionType::PipeSection:
        return pipeSectionStressPoints(input);
    case SectionType::GirderSection:
        return girderSectionStressPoints(input);
    }

    return {};
}

std::vector<StressPoint> StressPointDefaults::previewStressPoints(
    const SectionInput& input,
    const std::vector<StressPoint>& points)
{
    switch (input.type)
    {
    case SectionType::BoxSection:
        return boxPreviewPoints(input, points);
    case SectionType::GirderSection:
        return girderPreviewPoints(input, points);
    case SectionType::HSection:
    case SectionType::PipeSection:
        return points;
    }

    return {};
}
