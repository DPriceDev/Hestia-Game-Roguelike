//
// Created by David Price on 17/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DELAUNAY_TYPES_H
#define HESTIA_ROGUELIKE_DELAUNAY_TYPES_H

#include <cmath>
#include <vector>
#include <algorithm>

#include <maths/maths_types.h>

/**
 * Vertex
 */
struct Vertex {
    HGE::Pointf x, y;

    Vertex(HGE::Pointf x, HGE::Pointf y) : x(x), y(y) { }
    explicit Vertex(HGE::Vector2f location) : x(location.x), y(location.y) { }
    ~Vertex() = default;

    [[nodiscard]]
    HGE::Pointf magnitude() const {
        return sqrt((x * x) + (y * y));
    }

    [[nodiscard]]
    HGE::Vector2f asVector2f() const {
        return HGE::Vector2f(x, y);
    }
};

/**
 * Edge
 */
struct Edge {
    Vertex* a;
    Vertex* b;

    Edge(Vertex* a, Vertex* b) : a(a),  b(b) { }
    ~Edge() = default;
};

/**
 * Triangle
 */
struct Triangle {
    Vertex* a;
    Vertex* b;
    Vertex* c;

    Edge* edges[3]{ };

    Triangle(Vertex* a, Vertex* b, Vertex* c, Edge* ab, Edge* bc, Edge* ca) : a(a), b(b), c(c) {
        edges[0] = ab;
        edges[1] = bc;
        edges[2] = ca;
    }

    [[nodiscard]]
    HGE::Vector2f circumcenter() const {

        auto A = b->x - a->x;
        auto B = b->y - a->y;
        auto C = c->x - a->x;
        auto D = c->y - a->y;
        auto E = A * (a->x + b->x) + B * (a->y + b->y);
        auto F = C * (a->x + c->x) + D * (a->y + c->y);
        auto G = 2 * (A * (c->y - b->y) - B * (c->x - b->x));

        if(abs(G) < 0.000001) {
            auto minx = std::min({a->x, b->x, c->x});
            auto miny = std::min({a->y, b->y, c->y});
            auto dx = (std::max({a->x, b->x, c->x}) - minx) * 0.5;
            auto dy = (std::max({a->y, b->y, c->y}) - miny) * 0.5;

            return HGE::Vector2f(minx + dx, miny + dy);
        } else {
            return HGE::Vector2f((D*E - B*F) / G, (A*F - C*E) / G);
        }
    }

    [[nodiscard]]
    HGE::Pointf circumcenterRadius() const {
        auto circum = circumcenter();
        auto dx = circum.x - a->x;
        auto dy = circum.y - a->y;
        return sqrt(dx * dx + dy * dy);
    }
};

/**
 * Polygon
 */
struct Polygon {
    std::vector<Edge*> edges{ };
};

#endif //HESTIA_ROGUELIKE_DELAUNAY_TYPES_H
