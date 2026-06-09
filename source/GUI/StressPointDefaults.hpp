#pragma once

#include "../Geometry/SectionInput.hpp"
#include "../Geometry/SectionShape.hpp"

#include <vector>

struct StressPoint
{
    int id = 0;
    Point2D coordinates;
};

class StressPointDefaults
{
public:
    static std::vector<StressPoint> defaultStressPoints(const SectionInput& input);
    static std::vector<StressPoint> previewStressPoints(const SectionInput& input,
                                                        const std::vector<StressPoint>& points);
};
