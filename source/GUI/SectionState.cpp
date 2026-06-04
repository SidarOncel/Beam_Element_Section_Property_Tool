#include "SectionState.hpp"

#include <iomanip>
#include <sstream>

namespace
{
    void addError(ValidationResult& result, const std::string& message)
    {
        result.valid = false;
        result.errors.push_back(message);
    }

    bool isPositive(double value)
    {
        return value > 0.0;
    }

    double positiveOrFallback(double value, double fallback)
    {
        return isPositive(value) ? value : fallback;
    }

    std::string formatDouble(double value, int precision)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

    void validatePositive(ValidationResult& result, double value, const std::string& name)
    {
        if (!isPositive(value))
        {
            addError(result, name + " must be greater than zero.");
        }
    }

    void validateHSection(const SectionInput& input, ValidationResult& result)
    {
        validatePositive(result, input.width, "Width");
        validatePositive(result, input.height, "Height");
        validatePositive(result, input.flangeThickness, "Flange thickness");
        validatePositive(result, input.webThickness, "Web thickness");

        if (isPositive(input.height) && isPositive(input.flangeThickness) &&
            2.0 * input.flangeThickness >= input.height)
        {
            addError(result, "Twice the flange thickness must be less than the height.");
        }

        if (isPositive(input.width) && isPositive(input.webThickness) &&
            input.webThickness > input.width)
        {
            addError(result, "Web thickness cannot exceed the width.");
        }
    }

    void validateBoxSection(const SectionInput& input, ValidationResult& result)
    {
        validatePositive(result, input.width, "Width");
        validatePositive(result, input.height, "Height");

        const double top = positiveOrFallback(input.topFlangeThickness, input.wallThickness);
        const double bottom = positiveOrFallback(input.bottomFlangeThickness, input.wallThickness);
        const double left = positiveOrFallback(input.leftWebThickness, input.wallThickness);
        const double right = positiveOrFallback(input.rightWebThickness, input.wallThickness);

        validatePositive(result, top, "Top flange thickness");
        validatePositive(result, bottom, "Bottom flange thickness");
        validatePositive(result, left, "Left web thickness");
        validatePositive(result, right, "Right web thickness");

        if (isPositive(input.height) && top + bottom >= input.height)
        {
            addError(result, "Top and bottom thicknesses must be less than the height.");
        }

        if (isPositive(input.width) && left + right >= input.width)
        {
            addError(result, "Left and right thicknesses must be less than the width.");
        }
    }

    void validatePipeSection(const SectionInput& input, ValidationResult& result)
    {
        validatePositive(result, input.diameter, "Outer diameter");
        validatePositive(result, input.wallThickness, "Wall thickness");

        if (isPositive(input.diameter) && isPositive(input.wallThickness) &&
            2.0 * input.wallThickness >= input.diameter)
        {
            addError(result, "Twice the wall thickness must be less than the outer diameter.");
        }
    }

    void validateGirderSection(const SectionInput& input, ValidationResult& result)
    {
        validatePositive(result, input.A, "A");
        validatePositive(result, input.B, "B");
        validatePositive(result, input.G, "G");
        validatePositive(result, input.D, "D");
        validatePositive(result, input.e, "e");
        validatePositive(result, input.f, "f");
        validatePositive(result, input.H, "H");
        validatePositive(result, input.W, "W");
        validatePositive(result, input.M, "M");
        validatePositive(result, input.N, "N");
        validatePositive(result, input.p, "p");
        validatePositive(result, input.s, "s");
        validatePositive(result, input.t, "t");
        validatePositive(result, input.u, "u");
        validatePositive(result, input.M1, "M1");
        validatePositive(result, input.k, "k");
        validatePositive(result, input.k1, "k1");
        validatePositive(result, input.h, "h");
        validatePositive(result, input.h1, "h1");

        if (isPositive(input.H) && isPositive(input.W) && input.W >= input.H)
        {
            addError(result, "W must be less than H.");
        }

        if (isPositive(input.B) && isPositive(input.A) && input.A > input.B)
        {
            addError(result, "A cannot exceed B.");
        }

        if (isPositive(input.D) && isPositive(input.G) && input.G > input.D)
        {
            addError(result, "G cannot exceed D.");
        }
    }
}

const SectionInput& SectionState::input() const
{
    return m_input;
}

void SectionState::setInput(const SectionInput& input)
{
    m_input = input;
    m_hasProperties = false;
    m_lastValidation = validateInput(m_input);
}

void SectionState::setSectionType(SectionType type)
{
    m_input.type = type;
    m_hasProperties = false;
    m_lastValidation = validateInput(m_input);
}

ValidationResult SectionState::validate() const
{
    return validateInput(m_input);
}

bool SectionState::recalculate()
{
    m_lastValidation = validateInput(m_input);
    if (!m_lastValidation.valid)
    {
        m_hasProperties = false;
        return false;
    }

    m_properties = calculateProperties(m_input);
    m_hasProperties = true;
    return true;
}

bool SectionState::hasProperties() const
{
    return m_hasProperties;
}

const SectionProperties& SectionState::properties() const
{
    return m_properties;
}

const ValidationResult& SectionState::lastValidation() const
{
    return m_lastValidation;
}

std::vector<PropertyDisplayRow> SectionState::formattedPropertyRows(int precision) const
{
    if (!m_hasProperties)
    {
        return {};
    }

    return formatProperties(m_properties, precision);
}

ValidationResult SectionState::validateInput(const SectionInput& input)
{
    ValidationResult result;

    switch (input.type)
    {
    case SectionType::HSection:
        validateHSection(input, result);
        break;
    case SectionType::BoxSection:
        validateBoxSection(input, result);
        break;
    case SectionType::PipeSection:
        validatePipeSection(input, result);
        break;
    case SectionType::GirderSection:
        validateGirderSection(input, result);
        break;
    }

    return result;
}

SectionProperties SectionState::calculateProperties(const SectionInput& input)
{
    switch (input.type)
    {
    case SectionType::HSection:
        return PropertyCalculator::calculateHSectionProperties(input);
    case SectionType::BoxSection:
        return PropertyCalculator::calculateBoxSectionProperties(input);
    case SectionType::PipeSection:
        return PropertyCalculator::calculatePipeSectionProperties(input);
    case SectionType::GirderSection:
        return PropertyCalculator::calculateGirderSectionProperties(input);
    }

    return {};
}

std::vector<PropertyDisplayRow> SectionState::formatProperties(const SectionProperties& properties,
                                                               int precision)
{
    return {
        {"Area", formatDouble(properties.Area, precision), "mm^2"},
        {"cy", formatDouble(properties.cy, precision), "mm"},
        {"cz", formatDouble(properties.cz, precision), "mm"},
        {"Jz", formatDouble(properties.Jz, precision), "mm^4"},
        {"Jy", formatDouble(properties.Jy, precision), "mm^4"},
        {"Jyz", formatDouble(properties.Jyz, precision), "mm^4"},
        {"Jzo", formatDouble(properties.Jzo, precision), "mm^4"},
        {"Jyo", formatDouble(properties.Jyo, precision), "mm^4"},
        {"Jx", formatDouble(properties.Jx, precision), "mm^4"},
        {"Az", formatDouble(properties.Az, precision), "mm^2"},
        {"Ay", formatDouble(properties.Ay, precision), "mm^2"}
    };
}
