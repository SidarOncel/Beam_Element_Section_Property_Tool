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
    SectionType type = SectionType::HSection;

    // Common dimensions
    double width = 0.0;
    double height = 0.0; // NOTE: This represents the TOTAL outer height of the section

    // H-section specific
    double flangeThickness = 0.0;
    double webThickness = 0.0;

    // Box-section specific (supporting asymmetric thicknesses as per test data)
    double topFlangeThickness = 0.0;
    double bottomFlangeThickness = 0.0;
    double leftWebThickness = 0.0;
    double rightWebThickness = 0.0;

    // Pipe specific
    double diameter = 0.0;
    double wallThickness = 0.0;

    // Quayside Crane Girder specific (19 parameters from test data)
    double A = 0.0;
    double B = 0.0;
    double G = 0.0;
    double D = 0.0;
    double e = 0.0;
    double f = 0.0;
    double H = 0.0;
    double W = 0.0;
    double M = 0.0;
    double N = 0.0;
    double p = 0.0;
    double s = 0.0;
    double t = 0.0;
    double u = 0.0;
    double M1 = 0.0;
    double k = 0.0;
    double k1 = 0.0;
    double h = 0.0;
    double h1 = 0.0;

    // Optional general-purpose value for future extensions
    double extra = 0.0;
};