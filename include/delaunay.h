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

/**
 * Vertex
 */
struct Vertex {
    HGE::Pointf x, y;

    Vertex(HGE::Pointf x, HGE::Pointf y) : x(x), y(y) { }
    ~Vertex() = default;

    [[nodiscard]]
    HGE::Pointf magnitude() const {
        return sqrt((x * x) + (y * y));
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


    Triangle() = default;
    Triangle(Vertex* a, Vertex* b, Vertex* c, Edge* ab, Edge* bc, Edge* ca) : a(a), b(b), c(c) {
        edges[0] = ab;
        edges[1] = bc;
        edges[2] = ca;
    }
    ~Triangle() = default;

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

    /* todo: maybe unnesasary */
    Polygon() = default;
    ~Polygon() = default;
};

/**
 * Triangulation
 */
struct Triangulation {
    constexpr static float sSuperTriangleOffset = 30.0;

    std::vector<std::unique_ptr<Vertex>> mVertices;
    std::vector<std::unique_ptr<Edge>> mEdges;
    std::vector<std::unique_ptr<Triangle>> mTriangles;

    Edge* getEdgeFromVerts(const Vertex* a, const Vertex* b) {
        auto it = std::find_if(mEdges.begin(), mEdges.end(), [a, b] (auto & edge) {
           return (edge->a == a && edge->b == b) || (edge->b == a && edge->a == b);
        });

        if( it != mEdges.end()) {
            return it->get();
        } else {
            return nullptr;
        }
    }

    void deleteTriangle(Triangle* triangle) {
        auto it = std::find_if(mTriangles.begin(), mTriangles.end(), [&triangle] (const auto & tri) {
            return tri.get() == triangle;
        });
        if(it != mTriangles.end()) {
            mTriangles.erase(it);
        }
    }

    void deleteEdge(Edge* edge) {
        auto it = std::find_if(mEdges.begin(), mEdges.end(), [&edge] (const auto & e) {
            return e.get() == edge;
        });
        if(it != mEdges.end()) {
            mEdges.erase(it);
        }
    }

    static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices) {
        return *std::max_element(vertices.begin(), vertices.end(), [] (auto & a, auto & b) {
            return a.magnitude() < b.magnitude();
        });
    }

    void createSuperTriangleFromFurthestPoint(Vertex & furthest) {
        const static double adjacent = sqrt(3);
        const static double height = adjacent/2;
        auto radius = furthest.magnitude() + sSuperTriangleOffset;

        auto vertA = mVertices.emplace_back(std::make_unique<Vertex>(-(radius * adjacent), -radius)).get();
        auto vertB = mVertices.emplace_back(std::make_unique<Vertex>(radius * adjacent, -radius)).get();
        auto vertC = mVertices.emplace_back(std::make_unique<Vertex>( 0.0f , ((radius * 2) + (radius * adjacent)) * 0.5)).get();

        auto edgeAB = mEdges.emplace_back(std::make_unique<Edge>(vertA, vertB)).get();
        auto edgeBC = mEdges.emplace_back(std::make_unique<Edge>(vertB, vertC)).get();
        auto edgeCA = mEdges.emplace_back(std::make_unique<Edge>(vertC, vertA)).get();

        mTriangles.emplace_back(std::make_unique<Triangle>(vertA, vertB, vertC, edgeAB, edgeBC, edgeCA));
    }

    /**
     *
     * @param points
     */
    void triangulate(const std::vector<HGE::Vector2f>& points) {
        mTriangles.clear();
        mEdges.clear();
        mVertices.clear();

        auto vertices = std::vector<Vertex>();
        std::transform(points.begin(), points.end(), std::back_inserter(vertices), [] (const auto & point) {
            return Vertex(point.x, point.y);
        });

        /** create the super triangle from the furthest point */
        auto radius = getFurthestPointOfVertices(vertices);
        createSuperTriangleFromFurthestPoint(radius);

        /** for each vertex */
        for(const auto & vert: vertices) {
            auto badTriangles = std::vector<Triangle*>();

            /** check to see if the point is inside the circumcenter of this triangle */
           for(auto & triangle : mTriangles) {
               if(HGE::isPointInACircle({vert.x, vert.y}, triangle->circumcenter(), triangle->circumcenterRadius())) {
                   badTriangles.push_back(triangle.get());
               }
           }

           auto polygon = Polygon();
           auto edgesToDelete = std::vector<Edge*>();
           /** for each triangle. check if it shares a an edge with another triangle */
           for(auto & triangle : badTriangles) {

                for(auto & edge : triangle->edges) {
                    bool isNotShared = true;

                    for(auto & badTriangle : badTriangles) {

                        if(badTriangle != triangle) {

                            if(badTriangle->edges[0] == edge ||
                                badTriangle->edges[1] == edge ||
                                badTriangle->edges[2] == edge) {
                                isNotShared = false;
                            }

                        }
                    }

                    if(isNotShared) {
                        polygon.edges.push_back(edge);
                    } else {

                        auto it = std::find(edgesToDelete.begin(), edgesToDelete.end(), edge);

                        if(it == edgesToDelete.end()) {
                            edgesToDelete.push_back(edge);
                        }
                    }
                }
           }

           /** remove bad triangles */
           for(auto & triangle : badTriangles) {
               deleteTriangle(triangle);
           }

            for(auto & edge : edgesToDelete) {
                deleteEdge(edge);
            }


           /** for each edge, add a new triangle to the new vertex */
           // TODO: Rejig to make sure triangles are counter clockwise??
            auto newVert = mVertices.emplace_back(std::make_unique<Vertex>(vert.x, vert.y)).get();
            for(auto & edge : polygon.edges) {

                Edge* edgeNA;
                auto existingEdgeNA = getEdgeFromVerts(newVert, edge->a);
                if(existingEdgeNA != nullptr) {
                    edgeNA = existingEdgeNA;
                } else {
                    mEdges.emplace_back(std::make_unique<Edge>(newVert, edge->a));
                    edgeNA = mEdges.back().get();
                }

                Edge* edgeBN;
                auto existingEdgeBN = getEdgeFromVerts(edge->b, newVert);
                if(existingEdgeBN != nullptr) {
                    edgeBN = existingEdgeBN;
                } else {
                    mEdges.emplace_back(std::make_unique<Edge>(edge->b, newVert));
                    edgeBN = mEdges.back().get();
                }

                mTriangles.emplace_back(std::make_unique<Triangle>(newVert, edge->a, edge->b, edgeNA, edge, edgeBN));
            }


        }

        /* get all triangles linked to first 3 verts and delete them */




        mTriangles.erase(std::remove_if(mTriangles.begin(), mTriangles.end(), [&] (const auto & triangle) {
            return triangle->a == mVertices.at(0).get()
                || triangle->a == mVertices.at(1).get()
                || triangle->a == mVertices.at(2).get()
                || triangle->b == mVertices.at(0).get()
                || triangle->b == mVertices.at(1).get()
                || triangle->b == mVertices.at(2).get()
                || triangle->c == mVertices.at(0).get()
                || triangle->c == mVertices.at(1).get()
                || triangle->c == mVertices.at(2).get();
        }), mTriangles.end());

        /* get all edges linked to first 3 verts and delete them */
        mEdges.erase(std::remove_if(mEdges.begin(), mEdges.end(), [&] (const auto & edge) {
            return edge->a == mVertices.at(0).get()
                   || edge->a == mVertices.at(1).get()
                   || edge->a == mVertices.at(2).get()
                   || edge->b == mVertices.at(0).get()
                   || edge->b == mVertices.at(1).get()
                   || edge->b == mVertices.at(2).get();
        }), mEdges.end());

        /* delete first 3 verts */
        mVertices.erase(mVertices.begin() + 2);
        mVertices.erase(mVertices.begin() + 1);
        mVertices.erase(mVertices.begin() + 0);

    }
};

#endif //HESTIA_ROGUELIKE_DEPENDS_HGE_INCLUDE_MATHS_DELAUNAY_H
