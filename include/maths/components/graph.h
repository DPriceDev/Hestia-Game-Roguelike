//
// Created by David Price on 19/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_COMPONENTS_GRAPH_H
#define HESTIA_ROGUELIKE_MATHS_COMPONENTS_GRAPH_H

#include <vector>

#include "maths/components/connection.h"

struct Graph {
    std::vector<int> mPoints;
    std::vector<Connection> mConnections;

    Graph(std::vector<int> points, std::vector<Connection> connections)
            : mPoints(std::move(points)), mConnections(std::move(connections)) { }
};

#endif //HESTIA_ROGUELIKE_MATHS_COMPONENTS_GRAPH_H
