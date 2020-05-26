//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_TRIANGLE_H
#define HESTIA_ROGUELIKE_MATHS_TRIANGLE_H

#include <maths/maths_types.h>

#include "maths/components/edge.h"

struct Triangle {
    std::array<Vertex*, 3> mVertices;
    std::array<Edge*, 3> mEdges;
    HGE::Vector2f mCircumcenter;
    float mCircumradius;

    Triangle(Vertex* a, Vertex* b, Vertex* c, Edge* ab, Edge* bc, Edge* ca)
        : mVertices({a, b, c}),
          mEdges({ab, bc, ca}),
          mCircumcenter(circumcenter()),
          mCircumradius(circumcenterRadius(mCircumcenter)) { }

    [[nodiscard]] const Vertex* a() const noexcept { return mVertices[0]; }
    [[nodiscard]] const Vertex* b() const noexcept { return mVertices[1]; }
    [[nodiscard]] const Vertex* c() const noexcept { return mVertices[2]; }

    [[nodiscard]] const Edge* x() const noexcept { return mEdges[0]; }
    [[nodiscard]] const Edge* y() const noexcept { return mEdges[1]; }
    [[nodiscard]] const Edge* z() const noexcept { return mEdges[2]; }

private:
    [[nodiscard]] HGE::Vector2f circumcenter() const;
    [[nodiscard]] HGE::Pointf circumcenterRadius(HGE::Vector2f &circumcenter) const;
};

#endif //HESTIA_ROGUELIKE_MATHS_TRIANGLE_H
