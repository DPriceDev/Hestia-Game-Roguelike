//
// Created by David Price on 20/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_GRID_TILE_H
#define HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_GRID_TILE_H

#include "room.h"
#include "path.h"

enum class GridTileType {
    ROOM_FLOOR,
    WALL,
    DOOR,
    PATH,
    EMPTY
};

struct GridTile {
    GridTileType mType;
    Room *mRoomPtr;
    Path *mPathPtr;

    GridTile() : mType(GridTileType::EMPTY) { }

    GridTile(GridTileType type) : mType(type), mRoomPtr(nullptr), mPathPtr(nullptr) { }

    GridTile(GridTileType type, Room *room) : mType(type), mRoomPtr(room), mPathPtr(nullptr) { }

    GridTile(GridTileType type, Path *path) : mType(type), mRoomPtr(nullptr), mPathPtr(path) { }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_GRID_TILE_H
