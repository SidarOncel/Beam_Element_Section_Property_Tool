#include "BeamSectionAPI.h"
#include "../Core/PropertyCalculator.hpp"
#include "../Mesh/MeshGenerator.hpp"

int CalculateSectionProperties(const SectionInput* input, SectionProperties* out)
{
    if (!input || !out) return -1;

    switch (input->type)
    {
    case SectionType::HSection:
        *out = PropertyCalculator::calculateHSectionProperties(*input);
        break;
    case SectionType::BoxSection:
        *out = PropertyCalculator::calculateBoxSectionProperties(*input);
        break;
    case SectionType::PipeSection:
        *out = PropertyCalculator::calculatePipeSectionProperties(*input);
        break;
    case SectionType::GirderSection:
        *out = PropertyCalculator::calculateGirderSectionProperties(*input);
        break;
    default:
        return -2;   // unkown
    }
    return 0;
}

int GenerateMeshFromShape(const SectionShape* shape, double meshSize, MeshData* out)
{
    if (!shape || !out) return -1;

    if (shape->boundaryPoints.size() < 3)
        return -1;

    *out = MeshGenerator::generate(*shape, meshSize);
    if (!out->isValid)
        return -2;   // fail

    return 0;
}
