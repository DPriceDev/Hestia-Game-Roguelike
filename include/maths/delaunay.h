//
// Created by David Price on 16/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
#define HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H

#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>

#include <maths/maths.h>
#include <maths/maths_types.h>
#include <util/logger.h>

#include "maths/components/polygon.h"
#include "maths/components/triangle.h"
#include "maths/components/edge.h"
#include "maths/components/vertex.h"

#include "maths/components/triangulation.h"

/* Function Declarations */
static void removeSuperTriangle(Triangulation &triangulation);
static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices);
static void createSuperTriangleFromFurthestPoint(Triangulation &triangulation, const Vertex & furthest);

static std::pair<std::vector<Edge*>, Polygon> getSharedAndNonSharedEdgesOfTriangles(const std::vector<Triangle*>&triangles);

static void createTrianglesFromNewPoint(Triangulation &triangulation,
                                            const Vertex &vert,
                                            const Polygon &polygon);

/**
 * Delaunay Triangulation from Points
 *
 * This method parses a number of points with ids into a triangulation. it uses a delaunay process to insert each point
 * into the triangulation mesh.
 * @param points            - vector of 2d points and id's
 * @return Triangulation    - triangulation containing vertices, edges and triangles
 */
static Triangulation delaunayTriangulationFromPoints(const std::vector<std::pair<int, HGE::Vector2f>>& points) {
    Triangulation triangulation{ };

    auto const convertToVertices = [] (const std::pair<int, HGE::Vector2f> & point) {
        return Vertex(point.first, point.second);
    };

    std::vector<Vertex> vertices{ };
    std::transform(points.begin(), points.end(), std::back_inserter(vertices), convertToVertices);

    Vertex radius{ getFurthestPointOfVertices(vertices) };
    createSuperTriangleFromFurthestPoint(triangulation, radius);

    std::for_each(vertices.begin(), vertices.end(), [&triangulation] (const Vertex & vert) {
        auto badTriangles = std::vector<Triangle*>();
        auto polygon = Polygon();
        auto badEdges = std::vector<Edge*>();

        const auto isInTriangleCircumcenter = [&vert, &badTriangles] (const std::unique_ptr<Triangle> & triangle) {
            if(HGE::isPointInACircle(vert.mPosition, triangle->mCircumcenter, triangle->mCircumradius)) {
                badTriangles.push_back(triangle.get());
            }
        };

        std::for_each(triangulation.mTriangles.begin(), triangulation.mTriangles.end(), isInTriangleCircumcenter);

        std::tie(badEdges, polygon) = getSharedAndNonSharedEdgesOfTriangles(badTriangles);
        triangulation.deleteTrianglesAndEdges(badTriangles, badEdges);

        createTrianglesFromNewPoint(triangulation, vert, polygon);
    });

    removeSuperTriangle(triangulation);
    return std::move(triangulation);
}


/** retrieve and edge from a pair of vertices. */
static std::optional<Edge*> getEdgeFromVertices(const std::vector<std::unique_ptr<Edge>> &edges,
                                                const Vertex* a,
                                                const Vertex* b) {
    auto it = std::find_if(edges.begin(), edges.end(), [a, b] (auto & edge) {
       return (edge->a() == a && edge->b() == b) || (edge->b() == a && edge->a() == b);
    });
    if(it != edges.end()) {
        return it->get();
    } else {
        return std::nullopt;
    }
}

/** get the furthest point from the origin, in a set of vertices */
static Vertex getFurthestPointOfVertices(const std::vector<Vertex> &vertices) {
    return *std::max_element(vertices.begin(), vertices.end(), [] (auto & a, auto & b) {
        return a.mMagnitude < b.mMagnitude;
    });
}

/** create a super triangle, bigger than the furthest point. */
static void createSuperTriangleFromFurthestPoint(Triangulation &triangulation, const Vertex & furthest) {
    constexpr static float sSuperTriangleOffset = 30.0;
    const static double adjacent = sqrt(3);
    const static double height = adjacent/2;
    auto radius = furthest.mMagnitude + sSuperTriangleOffset;

    auto vertA = triangulation.mVertices.emplace_back(
            std::make_unique<Vertex>(-1, -(radius * adjacent), -radius)).get();
    auto vertB = triangulation.mVertices.emplace_back(
            std::make_unique<Vertex>(-2, radius * adjacent, -radius)).get();
    auto vertC = triangulation.mVertices.emplace_back(
            std::make_unique<Vertex>(-3, 0.0f , ((radius * 2) + (radius * adjacent)) * 0.5)).get();

    auto edgeAB = triangulation.mEdges.emplace_back(
            std::make_unique<Edge>(vertA, vertB)).get();
    auto edgeBC = triangulation.mEdges.emplace_back(
            std::make_unique<Edge>(vertB, vertC)).get();
    auto edgeCA = triangulation.mEdges.emplace_back(
            std::make_unique<Edge>(vertC, vertA)).get();

    triangulation.mTriangles.emplace_back(
            std::make_unique<Triangle>(vertA, vertB, vertC, edgeAB, edgeBC, edgeCA));
}

/** remove the super triangle and attached trangles, edges and vertices. */
static void removeSuperTriangle(Triangulation &triangulation) {
    triangulation.deleteVertex(-1);
    triangulation.deleteVertex(-2);
    triangulation.deleteVertex(-3);
}

/** adds non shared edges to a polygon, and shared edges of triangles in an edge array. */
static std::pair<std::vector<Edge*>, Polygon> getSharedAndNonSharedEdgesOfTriangles(
        const std::vector<Triangle*>&triangles) {

    std::map<Edge*, int> edgeMap{ };
    std::vector<Edge*> edges{ };
    Polygon polygon{ };

    const auto mapNumberOfEdges = [&edgeMap] (const Triangle* tri) {
        std::for_each(tri->mEdges.begin(), tri->mEdges.end(), [&edgeMap] (Edge* edge) {
            edgeMap[edge] += 1;
        });
    };

    const auto separateEdgesIfShared = [&edges, &polygon] (const std::pair<Edge*, int> pair) {
        if(pair.second == 1) {
            polygon.edges.push_back(pair.first);
        } else {
            edges.push_back(pair.first);
        }
    };

    std::for_each(triangles.begin(), triangles.end(), mapNumberOfEdges);
    std::for_each(edgeMap.begin(), edgeMap.end(), separateEdgesIfShared);
    return std::make_pair(std::move(edges), std::move(polygon));
}

/** Constructs triangles from a point and a polygon. */
static void createTrianglesFromNewPoint(Triangulation &triangulation,
                                 const Vertex &vert,
                                 const Polygon &polygon) {

    auto newVert = triangulation.mVertices.emplace_back(
            std::make_unique<Vertex>(vert)).get();

    for(auto & edge : polygon.edges) {
        Edge* edgeNA;
        Edge* edgeBN;
        auto existingEdgeNA = getEdgeFromVertices(triangulation.mEdges, newVert, edge->a());
        auto existingEdgeBN = getEdgeFromVertices(triangulation.mEdges, edge->b(), newVert);

        if(existingEdgeNA.has_value()) {
            edgeNA = existingEdgeNA.value();
        } else {
            edgeNA = triangulation.mEdges.emplace_back(
                    std::make_unique<Edge>(newVert, edge->a())).get();
        }

        if(existingEdgeBN.has_value()) {
            edgeBN = existingEdgeBN.value();
        } else {
            edgeBN = triangulation.mEdges.emplace_back(
                    std::make_unique<Edge>(edge->b(), newVert)).get();
        }

        triangulation.mTriangles.emplace_back(
                std::make_unique<Triangle>(newVert, edge->a(), edge->b(), edgeNA, edge, edgeBN));
    }
}

#endif //HESTIA_ROGUELIKE_DEPENDS_HGE_MATHS_DELAUNAY_H
