//
// Created by David Price on 20/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_DUNGEON_GRID_H
#define HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_DUNGEON_GRID_H

#include <vector>
#include <optional>
#include <algorithm>
#include <maths/maths_types.h>

#include "grid_tile.h"
#include "room.h"
#include "path.h"

class DungeonGrid {
    class Row {
        std::vector<std::unique_ptr<GridTile>> mColumns;
        int mOffsetX;

    public:
        Row(const int offset, const int size) : mOffsetX(offset) {
            mColumns.reserve(size);
            std::generate_n(std::back_inserter(mColumns), size, [] {
                return std::move(std::make_unique<GridTile>());
            });
        }

        GridTile* operator[](const int& index) const {
            return mColumns[index + mOffsetX].get();
        }
    };

    std::vector<Row> mGrid;
    int mOffsetX, mOffsetY;

public:
    /* todo: cant guarantee negative or positive bound numbers. add adjustments for this. */
    DungeonGrid(const int gridTop, const int gridRight, const int gridLeft = 0, const int gridBottom = 0)
            : mOffsetX(-gridLeft), mOffsetY(-gridBottom) {

        auto columnCount = gridRight - gridLeft;
        auto rowCount = gridTop - gridBottom;
        mGrid.reserve(rowCount);
        std::generate_n(std::back_inserter(mGrid), rowCount, [this, &columnCount] {
            return std::move(Row(mOffsetX, columnCount));
        });
    }

    void insertRooms(const std::vector<Room*>& rooms) {
    }

    void insertRoom(const Room* room) {
    }

    void insertPaths(const std::vector<Path*>& paths) {
    }

    void insertPath(const Path* path) {
    }

    Row& operator[](int index) {
        return mGrid[index + mOffsetY];
    }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_DUNGEON_GRID_H