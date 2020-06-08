//
// Created by David Price on 28/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_PATH_GENERATOR_H
#define HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_PATH_GENERATOR_H

#include <vector>
#include <random>

#include <maths/maths_types.h>
#include <maths/components/connection.h>

#include "room.h"
#include "path.h"
#include "grid_tile.h"

class PathGenerator {
    static auto pointOnRoomClosestToPoint(Room *room, HGE::Vector2i) -> HGE::Vector2i;
public:
    static auto generatePath(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                             const std::vector<std::unique_ptr<Room>> &rooms,
                             const Connection &connection) -> Path;
};

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_PATH_GENERATOR_H
