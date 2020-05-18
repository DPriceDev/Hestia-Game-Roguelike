//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_TRIANGLE_H
#define HESTIA_ROGUELIKE_MATHS_TRIANGLE_H

#include <cmath>
#include <vector>
#include <algorithm>
#include <array>

#include <maths/maths_types.h>

#include "edge.h"

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

    [[nodiscard]] const Vertex* a() const { return mVertices[0]; }
    [[nodiscard]] const Vertex* b() const { return mVertices[1]; }
    [[nodiscard]] const Vertex* c() const { return mVertices[2]; }

    [[nodiscard]] const Edge* x() const { return mEdges[0]; }
    [[nodiscard]] const Edge* y() const { return mEdges[1]; }
    [[nodiscard]] const Edge* z() const { return mEdges[2]; }

private:
    [[nodiscard]]
    HGE::Vector2f circumcenter() const {

        auto A = b()->x() - a()->x();
        auto B = b()->y() - a()->y();
        auto C = c()->x() - a()->x();
        auto D = c()->y() - a()->y();
        auto E = A * (a()->x() + b()->x()) + B * (a()->y() + b()->y());
        auto F = C * (a()->x() + c()->x()) + D * (a()->y() + c()->y());
        auto G = 2 * (A * (c()->y() - b()->y()) - B * (c()->x() - b()->x()));

        if(abs(G) < 0.000001) {
            auto minx = std::min({a()->x(), b()->x(), c()->x()});
            auto miny = std::min({a()->y(), b()->y(), c()->y()});
            auto dx = (std::max({a()->x(), b()->x(), c()->x()}) - minx) * 0.5;
            auto dy = (std::max({a()->y(), b()->y(), c()->y()}) - miny) * 0.5;

            return HGE::Vector2f(minx + dx, miny + dy);
        } else {
            return HGE::Vector2f((D*E - B*F) / G, (A*F - C*E) / G);
        }
    }

    [[nodiscard]]
    HGE::Pointf circumcenterRadius(HGE::Vector2f &circumcenter) const {
        auto dx = circumcenter.x - a()->x();
        auto dy = circumcenter.y - a()->y();
        return sqrt(dx * dx + dy * dy);
    }
};

#endif //HESTIA_ROGUELIKE_MATHS_TRIANGLE_H
