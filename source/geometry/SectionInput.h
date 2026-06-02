#pragma once

#include "SectionTypes.h"

/*
===============================================================================
Purpose:
Stores all geometric dimensions required to generate a cross-section.

This structure serves as the common input format exchanged between:
- GUI Module
- Geometry Module
- Property Calculation Module
- Stress Point Module
- FE Mesh Module
- DLL Interface

Design Philosophy:
A single input structure simplifies communication between modules and
reduces duplicate code.

Developer Notes:
Some parameters are only used by specific section types.

Example:
Pipe Section:
    diameter
    wallThickness

H Section:
    width
    height
    flangeThickness
    webThickness

Unused fields may remain zero.
===============================================================================
*/

struct SectionInput
{
    SectionType type;

    double width = 0.0;
    double height = 0.0;

    double flangeThickness = 0.0;
    double webThickness = 0.0;

    double wallThickness = 0.0;
    double diameter = 0.0;
};