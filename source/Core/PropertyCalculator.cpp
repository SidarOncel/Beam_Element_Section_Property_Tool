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
    
    // Support asymmetric thicknesses or fallback to single wallThickness
    double tt = input.topFlangeThickness > 0 ? input.topFlangeThickness : input.wallThickness;
    double tb = input.bottomFlangeThickness > 0 ? input.bottomFlangeThickness : input.wallThickness;
    double tl = input.leftWebThickness > 0 ? input.leftWebThickness : input.wallThickness;
    double tr = input.rightWebThickness > 0 ? input.rightWebThickness : input.wallThickness;

    // Dimensions of the inner void
    double h_inner = h - tt - tb;
    double b_inner = b - tl - tr;

    // Areas
    double A_out = b * h;
    double A_in = b_inner * h_inner;
    props.Area = A_out - A_in;

    // Void centroid relative to outer geometric center
    double cy_in = -b / 2.0 + tl + b_inner / 2.0;
    double cz_in = -h / 2.0 + tb + h_inner / 2.0;

    // Section Centroid
    props.cy = (A_out * 0.0 - A_in * cy_in) / props.Area;
    props.cz = (A_out * 0.0 - A_in * cz_in) / props.Area;

    // Moments of inertia about outer geometric center
    double Jy_out = (b * std::pow(h, 3)) / 12.0;
    double Jy_in = (b_inner * std::pow(h_inner, 3)) / 12.0 + A_in * std::pow(cz_in, 2);
    double Jy_geom = Jy_out - Jy_in;

    double Jz_out = (h * std::pow(b, 3)) / 12.0;
    double Jz_in = (h_inner * std::pow(b_inner, 3)) / 12.0 + A_in * std::pow(cy_in, 2);
    double Jz_geom = Jz_out - Jz_in;

    // Shift to actual centroid (Parallel axis theorem)
    props.Jy = Jy_geom - props.Area * std::pow(props.cz, 2);
    props.Jz = Jz_geom - props.Area * std::pow(props.cy, 2);

    // Principal axes (approximate assuming symmetry or small product of inertia)
    props.Jyo = props.Jy;
    props.Jzo = props.Jz;

    // Torsional moment of inertia (Bredt's formula for asymmetric thin-walled closed sections)
    double bm = b - (tl + tr) / 2.0;
    double hm = h - (tt + tb) / 2.0;
    double Am = bm * hm;
    
    // Line integral of ds / t
    double line_int = (bm / tt) + (bm / tb) + (hm / tl) + (hm / tr);
    props.Jx = (4.0 * std::pow(Am, 2)) / line_int;

    // Shear areas
    props.Az = h_inner * (tl + tr);
    props.Ay = b_inner * (tt + tb);

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
