//
// Created by David Price on 15/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
#define HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H

#include <vector>
#include <random>

#include <framework/systems/debug_system.h>

#include "room.h"
#include "path.h"
#include "grid_tile.h"

/**
 * Dungeon Generator
 */
class DungeonGenerator {
    const static int sNumberOfInitialRooms = 100;
    constexpr static float S_RADIUS = 100.0f;
    constexpr static float S_RADIUS_SQRD = S_RADIUS * S_RADIUS;
    constexpr static float S_TWO_PI = 2 * M_PI;
    constexpr static float sMinimumRoomSize = 6.0f;
    constexpr static float sMaximumRoomSize = 20.0f;
    constexpr static float sMinimumRoomArea = 220.0f;
    constexpr static float sSeperationFactor = 4.0f;
    const static int sGridSpacing = 5;

    struct Dungeon {
        std::vector<Room> mMainRooms;
        std::vector<Room> mMinorRooms;
        std::vector<Path> mPaths;
    };

    HGE::DebugComponent *mDebug;

    std::vector<std::unique_ptr<Room>> mRooms{ };
    std::random_device mRandomDevice{ };
    std::mt19937 mGenerator{ mRandomDevice() };
    std::uniform_real_distribution<float> mRadiusDistribution{ 0, S_RADIUS_SQRD };
    std::uniform_real_distribution<float> mAngleDistribution{ 0, S_TWO_PI };

    auto randomPointInCircle() -> HGE::Vector2i;

    static auto pointOnRoomClosestToPoint(Room *room, HGE::Vector2i) -> HGE::Vector2i;

    auto generateRandomRooms(int numberOfRooms) -> std::vector<std::unique_ptr<Room>>;

    static auto
    extractSmallAreaRoomsFromVector(std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>>;

    static auto separateRooms(std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>>;

    static auto
    createDungeonGridFromRooms(std::vector<std::unique_ptr<Room>> &rooms) -> HGE::Grid<std::unique_ptr<GridTile>>;

    static void insertRoomIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid, Room *room);

    static void insertRoomsIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                    std::vector<std::unique_ptr<Room>> &rooms);

public:
    explicit DungeonGenerator(HGE::DebugComponent *debug) : mDebug(debug) { }

    ~DungeonGenerator() = default;

    auto getRooms() -> const std::vector<std::unique_ptr<Room>> & {
        return mRooms;
    }

    void generate();
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
