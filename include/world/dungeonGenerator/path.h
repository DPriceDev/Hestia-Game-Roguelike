//
// Created by David Price on 20/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_PATH_H
#define HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_PATH_H

#include <utility>
#include <vector>
#include <maths/maths_types.h>

struct Path {
    std::vector<int> mConnectedRooms;
    std::vector<HGE::Vector2i> mNodes;

    Path(std::vector<HGE::Vector2i> nodes, std::vector<int> rooms)
        : mNodes(std::move(nodes)), mConnectedRooms(std::move(rooms)) { }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_PATH_H
