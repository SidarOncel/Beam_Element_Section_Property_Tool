#include "MeshGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

MeshData MeshGenerator::generate(const SectionShape& shape, double targetMeshSize)
{
    MeshData data;
    
    if (shape.boundaryPoints.size() < 3)
        return data;
    
    // Simple triangulation (ear clipping)
    std::vector<Point2D> points = shape.boundaryPoints;
    
    for (size_t i = 1; i < points.size() - 1; ++i)
    {
        MeshTriangle tri;
        tri.p1 = points[0];
        tri.p2 = points[i];
        tri.p3 = points[i + 1];
        data.triangles.push_back(tri);
    }
    
    // Collect vertices
    for (const auto& p : points)
        data.vertices.push_back(p);
    
    data.isValid = !data.triangles.empty();
    return data;
}