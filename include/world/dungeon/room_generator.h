//
// Created by David Price on 28/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_ROOM_GENERATOR_H
#define HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_ROOM_GENERATOR_H

#define _USE_MATH_DEFINES

#include <vector>
#include <random>
#include <memory>
#include <utility>
#include <cmath>
#include <math.h>

#include <maths/maths_types.h>

#include "room.h"
#include "path.h"
#include "grid_tile.h"

class RoomGenerator {
    constexpr static float sMinimumRoomSize = 6.0f;
    constexpr static float sMaximumRoomSize = 20.0f;
    constexpr static float sMinimumRoomArea = 220.0f;
    constexpr static float sSeperationFactor = 4.0f;

    constexpr static float sRadius = 100.0f;
    constexpr static float sRadiusSqrd = sRadius * sRadius;
    constexpr static float sTwoPi = 2 * M_PI;

    std::mt19937 mGenerator;

    struct GeneratedRooms {
        std::vector<std::unique_ptr<Room>> mRooms;
        std::vector<std::unique_ptr<Room>> mUnusedRooms;

        GeneratedRooms(std::vector<std::unique_ptr<Room>> rooms, std::vector<std::unique_ptr<Room>> unusedRooms)
            : mRooms(std::move(rooms)), mUnusedRooms(std::move(unusedRooms)) {}
    };

    auto randomPointInCircle() -> HGE::Vector2i;
    static auto separateRooms(std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>>;

    static auto extractSmallAreaRoomsFromVector(std::vector<std::unique_ptr<Room>> &rooms) ->
    std::vector<std::unique_ptr<Room>>;

public:
    auto generateRandomRooms(int numberOfRooms) -> std::vector<std::unique_ptr<Room>>;

    explicit RoomGenerator(std::mt19937 & generator) : mGenerator(generator) {}

    auto generateRooms(std::size_t roomCount) -> GeneratedRooms {
        auto rooms = generateRandomRooms(roomCount);
        rooms = separateRooms(rooms);
        auto unusedRooms = extractSmallAreaRoomsFromVector(rooms);
        return std::move(GeneratedRooms(std::move(rooms), std::move(unusedRooms)));
    }
};

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_ROOM_GENERATOR_H
