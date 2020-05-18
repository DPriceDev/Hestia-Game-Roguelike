//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_EDGE_H
#define HESTIA_ROGUELIKE_MATHS_EDGE_H

#include <cmath>

#include "vertex.h"

struct Edge {
    std::array<Vertex*, 2> mVertices;
    HGE::Pointf mLength;

    Edge(Vertex* a, Vertex* b) : mVertices({a, b}), mLength(length()) { }

    [[nodiscard]] Vertex* a() const { return mVertices[0]; }
    [[nodiscard]] Vertex* b() const { return mVertices[1]; }

private:
    [[nodiscard]]
    HGE::Pointf length() const {
        auto diff = a()->mPosition - b()->mPosition;
        return sqrt((diff.x * diff.x) + (diff.y * diff.y));
    }
};


#endif //HESTIA_ROGUELIKE_MATHS_EDGE_H
