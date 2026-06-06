#pragma once

#include "../geometry/SectionShape.hpp"
#include <vector>

struct MeshTriangle
{
    Point2D p1, p2, p3;
};

struct MeshData
{
    std::vector<MeshTriangle> triangles;
    std::vector<Point2D> vertices;
    bool isValid = false;
};

class MeshGenerator
{
public:
    static MeshData generate(const SectionShape& shape, double targetMeshSize);
};