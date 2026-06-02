#ifndef PROPERTY_CALCULATOR_HPP
#define PROPERTY_CALCULATOR_HPP

// SectionProperties struct as defined in Member2_Plan.md Phase 1
struct SectionProperties {
    // Area and centroid coordinates
    double Area;
    double cz;
    double cy;

    // Moments of inertia about Z and Y axes
    double Jz;
    double Jy;

    // Moments of inertia about centroidal principal axes
    double Jzo;
    double Jyo;

    // Torsional moment of inertia
    double Jx;

    // Shear areas in Z and Y directions
    double Az;
    double Ay;
};

// Forward declarations for Member 1's geometry classes
class HSection;
class BoxSection;
class PipeSection;
class CraneGirderSection;

class PropertyCalculator {
public:
    // Calculates properties for an H Section
    static SectionProperties calculateProperties(const HSection& section);

    // Calculates properties for a Box Section
    static SectionProperties calculateProperties(const BoxSection& section);

    // Calculates properties for a Pipe Section
    static SectionProperties calculateProperties(const PipeSection& section);

    // Calculates properties for a Quayside Crane Girder Section
    static SectionProperties calculateProperties(const CraneGirderSection& section);
};

#endif // PROPERTY_CALCULATOR_HPP
