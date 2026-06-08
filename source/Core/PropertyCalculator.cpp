#include "PropertyCalculator.hpp"
#include <cmath>
#include <vector>

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

    // Define the 9 primitive elements of the Quayside Crane Girder
    // Origin (0,0) is at the intersection of the left web centerline and bottom flange centerline.
    
    double sum_A = 0.0;
    double sum_Ay = 0.0;
    double sum_Az = 0.0;

    struct Element {
        double A, cy, cz, Iy, Iz, Iyz;
    };
    std::vector<Element> elements;

    auto addRect = [&](double w, double h, double cy, double cz) {
        double A = w * h;
        double Iy = (w * h * h * h) / 12.0;
        double Iz = (h * w * w * w) / 12.0;
        elements.push_back({A, cy, cz, Iy, Iz, 0.0});
        sum_A += A;
        sum_Ay += A * cy;
        sum_Az += A * cz;
    };

    auto addInclinedRect = [&](double L, double t, double cy, double cz, double dy, double dz) {
        double A = L * t;
        double sin_theta = dz / L;
        double cos_theta = dy / L;
        double Iy = (L * L * L * t / 12.0) * sin_theta * sin_theta + (L * t * t * t / 12.0) * cos_theta * cos_theta;
        double Iz = (L * L * L * t / 12.0) * cos_theta * cos_theta + (L * t * t * t / 12.0) * sin_theta * sin_theta;
        double Iyz = (L * L * L * t / 12.0) * sin_theta * cos_theta - (L * t * t * t / 12.0) * sin_theta * cos_theta;
        elements.push_back({A, cy, cz, Iy, Iz, Iyz});
        sum_A += A;
        sum_Ay += A * cy;
        sum_Az += A * cz;
    };

    // 1. Left Web
    addRect(input.t, input.H, 0.0, input.H / 2.0);

    // 2. Top Flange
    // Fix: cz is the center. Top flange spans from H - e to H, so center is H - e/2.0
    addRect(input.B, input.e, input.A / 2.0, input.H - input.e / 2.0); 

    // 3. Bottom Flange
    // Fix: cz is the center. Bottom flange spans from 0 to f, so center is f/2.0
    addRect(input.D, input.f, input.G / 2.0, input.f / 2.0);

    // 4. Right Web (Upper)
    addRect(input.u, input.H - input.W, input.A, input.W + (input.H - input.W) / 2.0);

    // 5. Right Web (Lower Inclined)
    double L_inc = std::sqrt(std::pow(input.A - input.G, 2) + std::pow(input.W, 2));
    addInclinedRect(L_inc, input.u, (input.G + input.A) / 2.0, input.W / 2.0, input.A - input.G, input.W);

    // 6. Internal T-stiffener Horizontal (h)
    addRect(input.h, input.h1, input.A - input.h / 2.0, input.W);

    // 7. Internal T-stiffener Vertical (k)
    addRect(input.k1, input.k, input.A - input.h, input.W);

    // 8. External T-stiffener Horizontal (M)
    double y_N = input.G + (input.A - input.G) * (input.N / input.W);
    addRect(input.M, input.p, y_N + input.M / 2.0, input.N);

    // 9. External T-stiffener Vertical (s)
    double y_s = y_N + input.M - input.M1;
    addRect(input.s, input.N, y_s, input.N / 2.0);

    // Global Centroid
    props.Area = sum_A;
    props.cy = sum_Ay / sum_A;
    props.cz = sum_Az / sum_A;

    // Global Moments of Inertia (Parallel Axis Theorem)
    props.Jy = 0.0;
    props.Jz = 0.0;
    props.Jyz = 0.0;

    for (const auto& el : elements) {
        double dy = el.cy - props.cy;
        double dz = el.cz - props.cz;
        props.Jy += el.Iy + el.A * dz * dz;
        props.Jz += el.Iz + el.A * dy * dy;
        props.Jyz += el.Iyz + el.A * dy * dz;
    }

    // Principal axes moments
    double avg = (props.Jy + props.Jz) / 2.0;
    double diff = (props.Jy - props.Jz) / 2.0;
    double R = std::sqrt(diff * diff + props.Jyz * props.Jyz);
    props.Jyo = avg + R;
    props.Jzo = avg - R;

    // Torsional Moment of Inertia (Bredt's + Open Branches)
    // Enclosed Area Am
    double Am = input.A * input.H - 0.5 * (input.A - input.G) * input.W;
    
    // Line integral ds/t for the closed loop
    double loop_integral = (input.H / input.t) + (input.A / input.e) + ((input.H - input.W) / input.u) + 
                           (L_inc / input.u) + (input.G / input.f);
    
    double Jx_closed = (4.0 * Am * Am) / loop_integral;

    auto addOpen = [&](double length, double thickness) {
        return (1.0 / 3.0) * length * thickness * thickness * thickness;
    };
    
    double Jx_open = 0.0;
    // Overhangs & Stiffeners
    Jx_open += addOpen((input.B - input.A) / 2.0, input.e) * 2.0; 
    Jx_open += addOpen((input.D - input.G) / 2.0, input.f) * 2.0; 
    Jx_open += addOpen(input.h, input.h1);
    Jx_open += addOpen(input.k, input.k1);
    Jx_open += addOpen(input.M, input.p);
    Jx_open += addOpen(input.N, input.s);

    props.Jx = Jx_closed + Jx_open;

    // Shear Areas
    props.Az = input.H * input.t + (input.H - input.W) * input.u + L_inc * input.u * std::pow(input.W / L_inc, 2);
    props.Ay = input.B * input.e + input.D * input.f;

    return props;
}
