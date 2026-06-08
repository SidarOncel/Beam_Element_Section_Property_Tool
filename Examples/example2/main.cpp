// box
#include <iostream>
#include "API.h"

int main()
{
    // 1. input
    SectionInput input{};
    input.type = SectionType::BoxSection;
    input.width  = 1320.0;
    input.height = 2600.0;
    input.topFlangeThickness    = 40.0;
    input.bottomFlangeThickness = 22.0;
    input.leftWebThickness      = 16.0;
    input.rightWebThickness     = 16.0;

    // 2. compute property
    SectionProperties props{};
    if (CalculateSectionProperties(&input, &props) != 0)
    {
        std::cerr << "ERROR: Property calculation failed.\n";
        return -1;
    }
    std::cout << "Box Section Area = " << props.Area << " mm^2\n";

    // 3. build
    SectionShape shape{};
    const double hw = input.width  / 2.0;
    const double hh = input.height / 2.0;
    shape.boundaryPoints = {
        {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}, {-hw, -hh}  // 闭合
    };
    shape.holes.push_back({
        {-hw + input.leftWebThickness,  -hh + input.bottomFlangeThickness},
        { hw - input.rightWebThickness, -hh + input.bottomFlangeThickness},
        { hw - input.rightWebThickness,  hh - input.topFlangeThickness},
        {-hw + input.leftWebThickness,   hh - input.topFlangeThickness},
        {-hw + input.leftWebThickness,  -hh + input.bottomFlangeThickness}
    });
    shape.isClosed = true;

    // 4. call DLL
    MeshData mesh{};
    if (GenerateMeshFromShape(&shape, 100.0, &mesh) != 0)
    {
        std::cerr << "ERROR: Mesh generation failed.\n";
        return -1;
    }

    // 5. output
    std::cout << "Mesh generated successfully:\n";
    std::cout << "  Number of triangles: " << mesh.triangles.size() << "\n";
    std::cout << "  Number of vertices : " << mesh.vertices.size()  << "\n";

    return 0;
}
