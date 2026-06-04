#pragma once

#include "../Core/PropertyCalculator.hpp"

#include <string>
#include <vector>

struct ValidationResult
{
    bool valid = true;
    std::vector<std::string> errors;
};

struct PropertyDisplayRow
{
    std::string name;
    std::string value;
    std::string unit;
};

class SectionState
{
public:
    const SectionInput& input() const;
    void setInput(const SectionInput& input);
    void setSectionType(SectionType type);

    ValidationResult validate() const;
    bool recalculate();

    bool hasProperties() const;
    const SectionProperties& properties() const;
    const ValidationResult& lastValidation() const;

    std::vector<PropertyDisplayRow> formattedPropertyRows(int precision = 2) const;

    static ValidationResult validateInput(const SectionInput& input);
    static SectionProperties calculateProperties(const SectionInput& input);
    static std::vector<PropertyDisplayRow> formatProperties(const SectionProperties& properties,
                                                            int precision = 2);

private:
    SectionInput m_input;
    SectionProperties m_properties;
    ValidationResult m_lastValidation;
    bool m_hasProperties = false;
};
