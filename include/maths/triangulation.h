//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H
#define HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H

// TODO: Create template for finding a pointer in a unique ptr vector

struct Triangulation {
    std::vector<std::unique_ptr<Vertex>> mVertices{ };
    std::vector<std::unique_ptr<Edge>> mEdges{ };
    std::vector<std::unique_ptr<Triangle>> mTriangles{ };

    /* Delete a triangle */
    void deleteTriangle(Triangle* triangle) {
        auto it = std::find_if(mTriangles.begin(), mTriangles.end(), [&triangle] (const auto & tri) {
            return tri.get() == triangle;
        });
        if(it != mTriangles.end()) {
            mTriangles.erase(it);
        }
    }

    /* Delete an edge */
    void deleteEdge(Edge* edge) {
        auto it = std::find_if(mEdges.begin(), mEdges.end(), [&edge] (const auto & e) {
            return e.get() == edge;
        });
        if(it != mEdges.end()) {
            mEdges.erase(it);
        }
    }

    /** Deletes a set of triangles and edges from the main vectors. */
    void deleteTrianglesAndEdges(const std::vector<Triangle *> &triangles, const std::vector<Edge *> &edges) {
        std::for_each(triangles.begin(), triangles.end(), [this] (auto & triangle) { deleteTriangle(triangle); });
        std::for_each(edges.begin(), edges.end(), [this] (auto & edge) { deleteEdge(edge); });
    }
};

#endif //HESTIA_ROGUELIKE_MATHS_TRIANGULATION_H
