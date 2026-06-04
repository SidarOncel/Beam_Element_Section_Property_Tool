#pragma once

#include "../geometry/SectionInput.hpp"
#include "../geometry/SectionShape.hpp"

class PreviewShapeAdapter
{
public:
    static SectionShape createShape(const SectionInput& input);
};
