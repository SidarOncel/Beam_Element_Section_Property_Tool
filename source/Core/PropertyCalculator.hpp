#pragma once

#include "../geometry/SectionInput.hpp"
#include <cmath>

// SectionProperties struct as defined in Member2_Plan.md Phase 1
struct SectionProperties {
    // Area and centroid coordinates
    double Area = 0.0;
    double cz = 0.0;
    double cy = 0.0;

    // Moments of inertia about Z and Y axes
    double Jz = 0.0;
    double Jy = 0.0;
    double Jyz = 0.0; // Product of inertia

    // Moments of inertia about centroidal principal axes
    double Jzo = 0.0;
    double Jyo = 0.0;

    // Torsional moment of inertia
    double Jx = 0.0;

    // Shear areas in Z and Y directions
    double Az = 0.0;
    double Ay = 0.0;
};

class PropertyCalculator {
public:
    // Calculates properties for an H Section
    static SectionProperties calculateHSectionProperties(const SectionInput& input);

    // Calculates properties for a Box Section
    static SectionProperties calculateBoxSectionProperties(const SectionInput& input);

    // Calculates properties for a Pipe Section
    static SectionProperties calculatePipeSectionProperties(const SectionInput& input);

    // Calculates properties for a Quayside Crane Girder Section
    static SectionProperties calculateGirderSectionProperties(const SectionInput& input);
};
