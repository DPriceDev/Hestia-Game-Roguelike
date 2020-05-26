//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H
#define HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H

#include <vector>

#include "triangle.h"
#include "edge.h"
#include "vertex.h"

struct Triangulation {
    std::vector<std::unique_ptr<Vertex>> mVertices{ };
    std::vector<std::unique_ptr<Edge>> mEdges{ };
    std::vector<std::unique_ptr<Triangle>> mTriangles{ };

    void deleteTriangle(const Triangle* triangle);
    void deleteEdge(const Edge* edge);
    void deleteVertex(const int id);
    void deleteTrianglesAndEdges(const std::vector<Triangle *> &triangles, const std::vector<Edge *> &edges);
};

#endif //HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H
