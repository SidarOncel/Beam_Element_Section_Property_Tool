#pragma once

#include <string>
#include "SectionInput.h"
#include "SectionShape.h"

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
    virtual ~SectionBase() = default;

    virtual bool validate(std::string& error) const = 0;

    virtual SectionShape generateShape() const = 0;
};