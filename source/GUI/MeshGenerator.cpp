#include "MeshGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <vector>

MeshData MeshGenerator::generate(const SectionShape& shape, double targetMeshSize)
{
    MeshData data;
    
    // Check if shape has at least 3 points (minimum for a triangle)
    if (shape.boundaryPoints.size() < 3)
        return data;
    
    // Get boundary points
    std::vector<Point2D> points = shape.boundaryPoints;
    
    // Calculate shape perimeter to determine number of subdivisions
    double perimeter = 0.0;
    for (size_t i = 0; i < points.size(); ++i)
    {
        const Point2D& current = points[i];
        const Point2D& next = points[(i + 1) % points.size()];
        double dx = next.y - current.y;
        double dz = next.z - current.z;
        perimeter += std::sqrt(dx * dx + dz * dz);
    }
    
    // Determine number of subdivisions based on target mesh size
    // Smaller mesh size = more subdivisions = finer mesh
    int subdivisions = 1;
    if (targetMeshSize > 0.0)
    {
        // Calculate how many segments we need along the perimeter
        double segmentLength = targetMeshSize;
        int requiredSegments = static_cast<int>(perimeter / segmentLength);
        subdivisions = std::max(1, requiredSegments / static_cast<int>(points.size()));
    }
    
    // Refine points based on mesh size
    std::vector<Point2D> refinedPoints;
    for (size_t i = 0; i < points.size(); ++i)
    {
        const Point2D& current = points[i];
        const Point2D& next = points[(i + 1) % points.size()];
        
        // Add current point
        refinedPoints.push_back(current);
        
        // Add intermediate points based on subdivision count
        for (int s = 1; s < subdivisions; ++s)
        {
            double t = static_cast<double>(s) / subdivisions;
            Point2D interpolated;
            interpolated.y = current.y + (next.y - current.y) * t;
            interpolated.z = current.z + (next.z - current.z) * t;
            refinedPoints.push_back(interpolated);
        }
    }
    
    // Use refined points for triangulation
    points = refinedPoints;
    
    // Simple ear clipping triangulation
    // Create triangles by connecting first point with consecutive pairs
    for (size_t i = 1; i < points.size() - 1; ++i)
    {
        MeshTriangle tri;
        tri.p1 = points[0];
        tri.p2 = points[i];
        tri.p3 = points[i + 1];
        data.triangles.push_back(tri);
    }
    
    // Collect all vertices
    for (const auto& p : points)
    {
        data.vertices.push_back(p);
    }
    
    data.isValid = !data.triangles.empty();
    return data;
}