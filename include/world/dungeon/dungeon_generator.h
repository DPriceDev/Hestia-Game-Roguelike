//
// Created by David Price on 15/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
#define HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H

#include <vector>
#include <random>
#include <memory>

#include <game/systems/debug_system.h>

#include "room.h"
#include "path.h"
#include "grid_tile.h"

/**
 * Dungeon Generator
 */
class DungeonGenerator {
    const static int sNumberOfInitialRooms = 100;

    const static int sGridSpacing = 5;

    struct Dungeon {
        std::vector<std::unique_ptr<Room>> mMainRooms;
        std::vector<std::unique_ptr<Room>> mMinorRooms;
        std::vector<Path> mPaths;
    };

    HGE::DebugComponent *mDebug;

    std::random_device mRandomDevice{ };
    std::mt19937 mGenerator{ mRandomDevice() };

    static auto
    createDungeonGridFromRooms(std::vector<std::unique_ptr<Room>> &rooms) -> HGE::Grid<std::unique_ptr<GridTile>>;

    static void insertRoomIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid, Room *room);

    static void insertRoomsIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                    std::vector<std::unique_ptr<Room>> &rooms);

public:
    explicit DungeonGenerator(HGE::DebugComponent *debug) : mDebug(debug) { }
    ~DungeonGenerator() = default;

    auto generate() -> Dungeon;
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
