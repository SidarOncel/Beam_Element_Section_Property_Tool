#include "PropertyCalculator.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SectionProperties PropertyCalculator::calculateHSectionProperties(const SectionInput& input) {
    SectionProperties props;
    double h = input.height;
    double b = input.width;
    double tf = input.flangeThickness;
    double tw = input.webThickness;

    // Area
    props.Area = 2.0 * b * tf + (h - 2.0 * tf) * tw;

    // Centroid (Origin is geometric center according to GeometryRules.md)
    props.cz = 0.0;
    props.cy = 0.0;

    // Moments of inertia
    double hw = h - 2.0 * tf; // web height
    props.Jy = 2.0 * ((b * std::pow(tf, 3)) / 12.0 + (b * tf) * std::pow((h - tf) / 2.0, 2)) 
               + (tw * std::pow(hw, 3)) / 12.0;
    props.Jz = 2.0 * ((tf * std::pow(b, 3)) / 12.0) 
               + (hw * std::pow(tw, 3)) / 12.0;

    // Principal axes
    props.Jyo = props.Jy;
    props.Jzo = props.Jz;

    // Torsional moment of inertia (approximate thin-walled)
    props.Jx = (1.0 / 3.0) * (2.0 * b * std::pow(tf, 3) + hw * std::pow(tw, 3));

    // Shear areas
    props.Az = hw * tw; // Vertical shear area (web)
    props.Ay = 2.0 * b * tf; // Horizontal shear area (flanges)

    return props;
}

SectionProperties PropertyCalculator::calculateBoxSectionProperties(const SectionInput& input) {
    SectionProperties props;
    double h = input.height;
    double b = input.width;
    double t = input.wallThickness; // Assumes uniform wall thickness as per SectionInput

    // Dimensions of the inner void
    double h_inner = h - 2.0 * t;
    double b_inner = b - 2.0 * t;

    // Area
    props.Area = (b * h) - (b_inner * h_inner);

    // Centroid
    props.cz = 0.0;
    props.cy = 0.0;

    // Moments of inertia
    props.Jy = (b * std::pow(h, 3)) / 12.0 - (b_inner * std::pow(h_inner, 3)) / 12.0;
    props.Jz = (h * std::pow(b, 3)) / 12.0 - (h_inner * std::pow(b_inner, 3)) / 12.0;

    // Principal axes
    props.Jyo = props.Jy;
    props.Jzo = props.Jz;

    // Torsional moment of inertia (Bredt's formula for thin-walled closed sections)
    double Am = (b - t) * (h - t);
    double U = 2.0 * ((b - t) + (h - t));
    props.Jx = (4.0 * std::pow(Am, 2) * t) / U;

    // Shear areas
    props.Az = 2.0 * h_inner * t;
    props.Ay = 2.0 * b_inner * t;

    return props;
}

SectionProperties PropertyCalculator::calculatePipeSectionProperties(const SectionInput& input) {
    SectionProperties props;
    double D = input.diameter;
    double t = input.wallThickness;
    
    double R_outer = D / 2.0;
    double R_inner = R_outer - t;

    // Area
    props.Area = M_PI * (std::pow(R_outer, 2) - std::pow(R_inner, 2));

    // Centroid
    props.cz = 0.0;
    props.cy = 0.0;

    // Moments of inertia
    props.Jy = (M_PI / 4.0) * (std::pow(R_outer, 4) - std::pow(R_inner, 4));
    props.Jz = props.Jy;

    // Principal axes
    props.Jyo = props.Jy;
    props.Jzo = props.Jz;

    // Torsional moment of inertia
    props.Jx = (M_PI / 2.0) * (std::pow(R_outer, 4) - std::pow(R_inner, 4));

    // Shear areas (approximation)
    props.Az = props.Area / 2.0;
    props.Ay = props.Area / 2.0;

    return props;
}

SectionProperties PropertyCalculator::calculateGirderSectionProperties(const SectionInput& input) {
    SectionProperties props;
    // TODO: Pending Member 1 geometry definitions (19 parameters) for the Quayside Crane Girder
    return props;
}
