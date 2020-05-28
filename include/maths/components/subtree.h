//
// Created by David Price on 19/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_COMPONENTS_SUBTREE_H
#define HESTIA_ROGUELIKE_MATHS_COMPONENTS_SUBTREE_H

#include "maths/components/connection.h"

struct SubTree {
    std::vector<int> mConnectedPoints;
    std::vector<Connection> mConnections;

    SubTree() = default;

    explicit SubTree(int initialPoint) : mConnectedPoints({ initialPoint }) { }

    SubTree &operator+=(const SubTree &other) {
        this->mConnectedPoints.insert(this->mConnectedPoints.end(),
                                      other.mConnectedPoints.begin(),
                                      other.mConnectedPoints.end());
        this->mConnections.insert(this->mConnections.end(),
                                  other.mConnections.begin(),
                                  other.mConnections.end());
        return *this;
    }

    SubTree &operator=(const SubTree &other) = default;
};

#endif //HESTIA_ROGUELIKE_INCLUDE_MATHS_COMPONENTS_SUBTREE_H
