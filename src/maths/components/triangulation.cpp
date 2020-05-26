//
// Created by David Price on 26/05/2020.
//

#include "maths/components/triangulation.h"

#include <algorithm>
#include <util/pointer_helper.h>

/* Delete a triangle */
void Triangulation::deleteTriangle(const Triangle* triangle) {
    auto it = HGE::doesContainPointer(mTriangles, triangle);
    mTriangles.erase(it);
}

/* Delete an edge */
void Triangulation::deleteEdge(const Edge* edge) {
    auto it = HGE::doesContainPointer(mEdges, edge);
    mEdges.erase(it);
}

/** */
void Triangulation::deleteVertex(const int id) {
    const auto vertexIdMatch = [id] (const auto & vertex) {
        return vertex->mId == id;
    };

    const auto triIt = std::remove_if(mTriangles.begin(), mTriangles.end(), [id, vertexIdMatch] (const auto &triangle) {
        return std::any_of(triangle->mVertices.begin(), triangle->mVertices.end(), vertexIdMatch);
    });
    mTriangles.erase(triIt, mTriangles.end());

    const auto edgeIt = std::remove_if(mEdges.begin(), mEdges.end(), [id, vertexIdMatch] (const auto &edge) {
        return std::any_of(edge->mVertices.begin(), edge->mVertices.end(), vertexIdMatch);
    });
    mEdges.erase(edgeIt, mEdges.end());

    const auto vertIt = std::remove_if(mVertices.begin(), mVertices.end(), vertexIdMatch);
    mVertices.erase(vertIt, mVertices.end());
}

/** Deletes a set of triangles and edges from the main vectors. */
void Triangulation::deleteTrianglesAndEdges(const std::vector<Triangle *> &triangles, const std::vector<Edge *> &edges) {
    std::for_each(triangles.begin(), triangles.end(), [this] (auto & triangle) { deleteTriangle(triangle); });
    std::for_each(edges.begin(), edges.end(), [this] (auto & edge) { deleteEdge(edge); });
}
