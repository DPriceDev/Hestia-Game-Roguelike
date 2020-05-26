//
// Created by David Price on 17/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H
#define HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H

#include "maths/components/triangulation.h"
#include "maths/components/graph.h"

class MinimumSpanningTree {

public:
    static Graph treeFromDelaunayTriangulation(const Triangulation &triangulation);
};

#endif //HESTIA_ROGUELIKE_MINIMUM_SPANNING_TREE_H
