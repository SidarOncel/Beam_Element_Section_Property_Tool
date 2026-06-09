#pragma once

#include "../Geometry/SectionInput.hpp"
#include "../Geometry/SectionShape.hpp"

class PreviewShapeAdapter
{
public:
    static SectionShape createShape(const SectionInput& input);
};
