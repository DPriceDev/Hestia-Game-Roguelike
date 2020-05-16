//
// Created by David Price on 16/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
#define HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H

#include <cmath>
#include <vector>
#include <algorithm>

#include <maths/maths_types.h>
#include <util/logger.h>

struct Vertex;
struct Triangle;

struct Vertex {
    HGE::Pointd x, y;

    Vertex(HGE::Pointd x, HGE::Pointd y) : x(x), y(y) { }
    ~Vertex() = default;

    [[nodiscard]]
    HGE::Pointd magnitude() const {
        return sqrt((x * x) + (y * y));
    }
};

struct Triangle {
    Vertex* a;
    Vertex* b;
    Vertex* c;

    Triangle() = default;
    Triangle(Vertex* a, Vertex* b, Vertex* c) : a(a), b(b), c(c) { }
    ~Triangle() = default;

    [[nodiscard]]
    HGE::Vector2d circumcenter() const {

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

            return HGE::Vector2d(minx + dx, miny + dy);
        } else {
            return HGE::Vector2d((D*E - B*F) / G, (A*F - C*E) / G);
        }
    }

    [[nodiscard]]
    HGE::Pointd circumcenterRadius() const {
        auto circum = circumcenter();
        auto dx = circum.x - a->x;
        auto dy = circum.y - a->y;
        return sqrt(dx * dx + dy * dy);
    }
};

/**
 * Triangulation
 */
struct Triangulation {
    constexpr static float sSuperTriangleOffset = 30.0;

    std::vector<std::unique_ptr<Vertex>> mVertices;
    std::vector<Triangle> mTriangles;

    static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices) {
        return *std::max_element(vertices.begin(), vertices.end(), [] (auto & a, auto & b) {
            return a.magnitude() < b.magnitude();
        });
    }

    void createSuperTriangleFromFurthestPoint(Vertex & furthest) {
        const static double adjacent = sqrt(3);
        const static double height = adjacent/2;
        auto radius = furthest.magnitude(); // + sSuperTriangleOffset;

        auto bottomRight = mVertices.emplace_back(std::make_unique<Vertex>(radius * adjacent, -radius)).get();
        auto bottomLeft = mVertices.emplace_back(std::make_unique<Vertex>(-(radius * adjacent), -radius)).get();

        auto top = mVertices.emplace_back(std::make_unique<Vertex>( 0.0f , ((radius * 2) + (radius * adjacent)) * 0.5))
                .get();
        mTriangles.emplace_back(Triangle(bottomLeft, bottomRight, top));
    }

    void triangulate(const std::vector<HGE::Vector2f>& points) {
        mTriangles.clear();
        mVertices.clear();

        auto vertices = std::vector<Vertex>();
        std::transform(points.begin(), points.end(), std::back_inserter(vertices), [] (const auto & point) {
            return Vertex(point.x, point.y);
        });

       auto radius = getFurthestPointOfVertices(vertices);
       createSuperTriangleFromFurthestPoint(radius);



//        radius = get furthest point();
//        superTriangle = radiusToTriangle();
//
//        triangulation.add_triangle(superTriangle);
//
//        for(auto & vertex : mVertices) {
//            std::vector<triangle> badTriangles{ };
//
//            for(auto & triangle : triangulation.triangles) {
//
//                if(vertexIsInTriangleCircumfrence(vertex, triangle)) {
//                    badTriangles.push_back(triangle);
//                }
//
//
//
//
//
//
//            }
//
//
//
//
//
//        }

    }
};

#endif //HESTIA_ROGUELIKE_DEPENDS_HGE_INCLUDE_MATHS_DELAUNAY_H
