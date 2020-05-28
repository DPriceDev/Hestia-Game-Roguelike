//
// Created by David Price on 16/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
#define HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H

#include <vector>
#include <utility>
#include <optional>

#include "maths/components/triangulation.h"
#include "maths/components/polygon.h"

class Delaunay {
    static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices);

    static void createSuperTriangleFromFurthestPoint(Triangulation &triangulation, const Vertex &furthest);

    static void removeSuperTriangle(Triangulation &triangulation);

    static std::optional<Edge *> getEdgeFromVertices(const std::vector<std::unique_ptr<Edge>> &edges,
                                                     const Vertex *a, const Vertex *b);

    static std::pair<std::vector<Edge *>, Polygon> getSharedAndNonSharedEdgesOfTriangles(
            const std::vector<Triangle *> &triangles);

    static void createTrianglesFromNewPoint(Triangulation &triangulation,
                                            const Vertex &vert,
                                            const Polygon &polygon);

public:
    static Triangulation triangulationFromPoints(const std::vector<std::pair<int, HGE::Vector2f>> &points);
};

#endif //HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
