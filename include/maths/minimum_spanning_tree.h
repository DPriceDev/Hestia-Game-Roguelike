//
// Created by David Price on 17/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H
#define HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H

#include <utility>
#include <vector>
#include <memory>
#include <random>

#include <maths/maths_types.h>

#include "maths/components/graph.h"
#include "maths/components/subtree.h"
#include "maths/components/connection.h"
#include "components/triangulation.h"
#include "components/vertex.h"
#include "components/edge.h"

static Graph minimumSpanningTreeFromDelaunayTriangulation(const Triangulation &triangulation) {

    std::vector<std::unique_ptr<SubTree>> pTrees;
    const auto toTree = [&pTrees] (const auto & vertex) {
        auto ptr = pTrees.emplace_back(std::make_unique<SubTree>(SubTree(vertex->mId))).get();
        return std::make_pair(vertex->mId, ptr);
    };

    const auto toConnection = [] (const auto & edge) {
        return Connection(edge->a()->mId, edge->b()->mId, edge->mLength);
    };

    const auto byShortestLength = [] (const auto & edgeA, const auto & edgeB) {
        return edgeA.mLength < edgeB.mLength;
    };

    std::map<int, SubTree*> trees;
    std::transform(triangulation.mVertices.begin(), triangulation.mVertices.end(),
            std::inserter(trees, trees.end()), toTree);

    std::vector<Connection> edges;
    std::transform(triangulation.mEdges.begin(), triangulation.mEdges.end(),
            std::back_inserter(edges), toConnection);
    std::sort(edges.begin(), edges.end(), byShortestLength);

    LOG_DEBUG("Minimum Spanning Tree", "tree count: ", trees.size(), "edge count: ", edges.size())

    /* for each edge */
    auto connectionsLeft = trees.size();
    auto offset = 0;
    while(connectionsLeft > 0 && offset < edges.size()) {
        auto edge = *(edges.begin() + offset);

        /* if not, select the first tree and combine the connect vertex vectors */
        if(trees.at(edge.mA) != trees.at(edge.mB)) {
            *trees.at(edge.mA) += *trees.at(edge.mB);
            trees.at(edge.mA)->mConnections.push_back(edge);

            for(auto & id : trees.at(edge.mA)->mConnectedPoints) {
                trees.at(id) = trees.at(edge.mA);
            }

            edges.erase(edges.begin());

            --connectionsLeft;
        } else {
            ++offset;
        }
    }

    int extraConnectionSize = floor(0.15f * edges.size());
    auto graph = Graph(trees.begin()->second->mConnectedPoints, trees.begin()->second->mConnections);

    auto randomEdges = std::sample(edges.begin(), edges.end(), std::back_inserter(graph.mConnections),
            extraConnectionSize, std::mt19937{ std::random_device{}()});

    return std::move(graph);
}

#endif //HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H
