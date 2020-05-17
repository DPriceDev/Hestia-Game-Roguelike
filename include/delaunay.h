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

#include "delaunay_types.h"

/**
 * Triangulation
 */
class Triangulation {
    std::vector<std::unique_ptr<Vertex>> mVertices;
    std::vector<std::unique_ptr<Edge>> mEdges;
    std::vector<std::unique_ptr<Triangle>> mTriangles;

    /* retrieve and edge from a pair of vertices. */
    std::optional<Edge*> getEdgeFromVertices(const Vertex* a, const Vertex* b) {
        auto it = std::find_if(mEdges.begin(), mEdges.end(), [a, b] (auto & edge) {
           return (edge->a == a && edge->b == b) || (edge->b == a && edge->a == b);
        });
        if(it != mEdges.end()) {
            return it->get();
        } else {
            return std::nullopt;
        }
    }

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

    /** get the furthest point from the origin, in a set of vertices */
    static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices) {
        return *std::max_element(vertices.begin(), vertices.end(), [] (auto & a, auto & b) {
            return a.magnitude() < b.magnitude();
        });
    }

    /** create a super triangle, bigger than the furthest point. */
    void createSuperTriangleFromFurthestPoint(Vertex & furthest) {
        constexpr static float sSuperTriangleOffset = 30.0;
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

    /** remove the super triangle and attached trangles, edges and vertices. */
    void removeSuperTriangle() {
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

    /** adds non shared edges to a polygon, and shared edges of triangles in an edge array. */
    static void getSharedAndNonSharedEdgesOfTriangles(const std::vector<Triangle*> &triangles,
                                                      std::vector<Edge*> &edges,
                                                      Polygon &polygon) {
        for(auto & triangle : triangles) {
            for(auto & edge : triangle->edges) {
                bool isNotShared = true;
                for(auto & badTriangle : triangles) {
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
                    auto it = std::find(edges.begin(), edges.end(), edge);
                    if(it == edges.end()) {
                        edges.push_back(edge);
                    }
                }
            }
        }
    }

    /** Constructs triangles from a point and a polygon. */
    void createTrianglesFromNewPoint(const Vertex &vert, const Polygon &polygon) {
        auto newVert = mVertices.emplace_back(std::__1::make_unique<Vertex>(vert.x, vert.y)).get();
        for(auto & edge : polygon.edges) {

            Edge* edgeNA;
            Edge* edgeBN;
            auto existingEdgeNA = getEdgeFromVertices(newVert, edge->a);
            auto existingEdgeBN = getEdgeFromVertices(edge->b, newVert);

            if(existingEdgeNA.has_value()) {
                edgeNA = existingEdgeNA.value();
            } else {
                mEdges.emplace_back(std::__1::make_unique<Edge>(newVert, edge->a));
                edgeNA = mEdges.back().get();
            }

            if(existingEdgeBN.has_value()) {
                edgeBN = existingEdgeBN.value();
            } else {
                mEdges.emplace_back(std::__1::make_unique<Edge>(edge->b, newVert));
                edgeBN = mEdges.back().get();
            }

            mTriangles.emplace_back(std::__1::make_unique<Triangle>(newVert, edge->a, edge->b,edgeNA, edge, edgeBN));
        }
    }

public:

    /**
     * Triangulation Constructor
     * Takes a group of points and constructs the triangulation.
     * @param points
     */
    explicit Triangulation(const std::vector<HGE::Vector2f>& points) {
        auto vertices = std::vector<Vertex>();
        std::transform(points.begin(), points.end(), std::back_inserter(vertices), [] (const auto & point) {
            return Vertex(point);
        });

        auto radius = getFurthestPointOfVertices(vertices);
        createSuperTriangleFromFurthestPoint(radius);

        for(const auto & vert: vertices) {
            auto badTriangles = std::vector<Triangle*>();

            for(auto & triangle : mTriangles) {
               if(HGE::isPointInACircle(vert.asVector2f(), triangle->circumcenter(), triangle->circumcenterRadius())) {
                   badTriangles.push_back(triangle.get());
               }
            }

            auto polygon = Polygon();
            auto badEdges = std::vector<Edge*>();
            getSharedAndNonSharedEdgesOfTriangles(badTriangles, badEdges, polygon);
            deleteTrianglesAndEdges(badTriangles, badEdges);
            createTrianglesFromNewPoint(vert, polygon);
        }

        removeSuperTriangle();
    }

    /**
     * Get Triangles
     * @return vector of triangle pointers
     */
    std::vector<Triangle*> getTriangles() {
        auto triangles = std::vector<Triangle*>();
        std::transform(mTriangles.begin(), mTriangles.end(), std::back_inserter(triangles),
                [] (const auto & triangle) { return triangle.get(); });
        return triangles;
    }

    /**
     * Get Edges
     * @return vector of Edge pointers
     */
    std::vector<Edge*> getEdges() {
        auto edges = std::vector<Edge*>();
        std::transform(mEdges.begin(), mEdges.end(), std::back_inserter(edges),
                       [] (const auto & edge) { return edge.get(); });
        return edges;
    }

    /**
     * Get Vertices
     * @return vector of Vertex pointers
     */
    std::vector<Vertex*> getVertices() {
        auto vertices = std::vector<Vertex*>();
        std::transform(mVertices.begin(), mVertices.end(), std::back_inserter(vertices),
                       [] (const auto & vertex) { return vertex.get(); });
        return vertices;
    }
};

#endif //HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
