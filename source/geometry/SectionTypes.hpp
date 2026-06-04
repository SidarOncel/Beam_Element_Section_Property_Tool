#pragma once

/*
===============================================================================
File: SectionTypes.hpp

Purpose:
Defines all cross-section types currently supported by the Beam Element
Section Property Tool.

This enum acts as the primary identifier used throughout the software.
GUI components, geometry generation, property calculations, stress point
generation, FE mesh creation, and DLL interfaces should all reference
these values when determining section behavior.

Supported Sections:
- H Section
- Box Section
- Pipe Section
- Quayside Crane Girder Section

Developer Notes:
When adding a new section type in the future, update this enum first and
then implement the corresponding geometry, calculation, GUI, and export
logic.

===============================================================================
*/

enum class SectionType
{
    HSection,
    BoxSection,
    PipeSection,
    GirderSection
};

inline const char* toString(SectionType type)
{
    switch (type)
    {
    case SectionType::HSection:   return "H Section";
    case SectionType::BoxSection:  return "Box Section";
    case SectionType::PipeSection: return "Pipe Section";
    case SectionType::GirderSection: return "Girder Section";
    default:                       return "Unknown";
    }
}
