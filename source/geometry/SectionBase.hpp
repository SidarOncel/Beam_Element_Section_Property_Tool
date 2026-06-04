#pragma once

#include <string>
#include "SectionInput.hpp"
#include "SectionShape.hpp"

/*
===============================================================================
File: SectionBase.h

Purpose:
Provides the abstract base interface for all section geometries.

All section implementations must inherit from this class.

Examples:
- HSection
- BoxSection
- PipeSection
- GirderSection

Responsibilities:
1. Validate user input.
2. Generate geometric representation.
3. Provide a common interface for future modules.

Developer Notes:
Adding a new section type requires implementing all pure virtual
functions defined below.

===============================================================================
*/

class SectionBase
{
public:
    explicit SectionBase(const SectionInput& input) : m_input(input) {}
    virtual ~SectionBase() = default;

    virtual bool validate(std::string& errorMessage) const = 0;
    virtual SectionShape generateShape() const = 0;

    const SectionInput& input() const { return m_input; }

protected:
    SectionInput m_input;
};
