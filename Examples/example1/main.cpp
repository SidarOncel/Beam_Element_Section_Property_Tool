// H-shape
#include <iostream>
#include "../../Source/API/API.h"

int main()
{
    // 1. input
    SectionInput input{};
    input.type = SectionType::HSection;
    input.width  = 200.0;    // mm
    input.height = 300.0;
    input.flangeThickness = 15.0;
    input.webThickness     = 10.0;

    // 2. call DLL
    SectionProperties props{};
    if (CalculateSectionProperties(&input, &props) != 0)
    {
        std::cerr << "ERROR: Failed to calculate section properties.\n";
        return -1;
    }

    // 3. output
    std::cout << "=== H-Section Properties ===\n";
    std::cout << "Area  : " << props.Area << " mm^2\n";
    std::cout << "Iy    : " << props.Jy   << " mm^4\n";
    std::cout << "Iz    : " << props.Jz   << " mm^4\n";
    std::cout << "Jx (Torsion) : " << props.Jx << " mm^4\n";
    std::cout << "Ay (Shear)   : " << props.Ay << " mm^2\n";
    std::cout << "Az (Shear)   : " << props.Az << " mm^2\n";

    return 0;
}
