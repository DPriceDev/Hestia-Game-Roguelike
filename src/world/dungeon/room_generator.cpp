//
// Created by David Price on 28/05/2020.
//

#include "world/dungeon/room_generator.h"

#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

#include "world/dungeon/breadth_path_generator.h"

/**
 *
 * @return
 */
auto RoomGenerator::randomPointInCircle() -> HGE::Vector2i {
    std::uniform_real_distribution<float> mRadiusDistribution{ 0, sRadiusSqrd };
    std::uniform_real_distribution<float> mAngleDistribution{ 0, sTwoPi };
    float radius = std::sqrt(mRadiusDistribution(mGenerator));
    float angle = mAngleDistribution(mGenerator);
    return { static_cast<int>(radius * cos(angle)),
             static_cast<int>(radius * sin(angle)) };
}

/**
 *
 * @param rooms
 * @return
 */
auto RoomGenerator::separateRooms(std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>> {

    std::vector<Room *> pRooms;
    std::transform(rooms.begin(), rooms.end(), std::back_inserter(pRooms), [](const auto &room) {
        return room.get();
    });

    bool overlapsExist = true;
    while (overlapsExist) {
        overlapsExist = false;

        std::for_each(pRooms.begin(), pRooms.end(), [&](auto &room) {
            std::vector<Room *> overlapping{ }, overlappingSame{ };

            std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlapping), [&](auto &otherRoom) {
                return room->mRect.isOverlapping(otherRoom->mRect)
                       && room != otherRoom
                       && room->mRect.midpoint() != otherRoom->mRect.midpoint();
            });

            std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlappingSame), [&](auto &otherRoom) {
                return room->mRect.isOverlapping(otherRoom->mRect)
                       && room != otherRoom
                       && room->mRect.midpoint() == otherRoom->mRect.midpoint();
            });

            const auto matchSeparate = [&room, &overlapsExist](auto &a, auto &b) {
                overlapsExist = true;
                auto midpoint = (room->mRect.midpoint() - b->mRect.midpoint()) / sSeperationFactor;
                midpoint.x = ceil(midpoint.x);
                midpoint.y = ceil(midpoint.y);
                return a + midpoint;
            };

            const auto matchSame = [&room, &overlapsExist](auto &a, auto &b) {
                overlapsExist = true;
                auto midpointNorm = room->mRect.midpoint().normalised();
                midpointNorm.x = ceil(midpointNorm.x);
                midpointNorm.y = ceil(midpointNorm.y);
                return a + midpointNorm;
            };

            room->mMovement += std::accumulate(overlapping.begin(),
                                               overlapping.end(),
                                               HGE::Vector2i(),
                                               matchSeparate);

            room->mMovement += std::accumulate(overlappingSame.begin(),
                                               overlappingSame.end(),
                                               HGE::Vector2i(),
                                               matchSame);
        });

        std::for_each(pRooms.begin(), pRooms.end(), [](auto &room) {
            room->mRect.position() += room->mMovement;
            room->mMovement = HGE::Vector2i();
        });
    }

    return std::move(rooms);
}

/**
 *
 * @param numberOfRooms
 * @return
 */
std::vector<std::unique_ptr<Room>> RoomGenerator::generateRandomRooms(const int numberOfRooms) {
    std::vector<std::unique_ptr<Room>> rooms{ };

    for (int i = 0; i < numberOfRooms; ++i) {
        auto width = HGE::randomNumberBetween<int>(sMinimumRoomSize, sMaximumRoomSize);

        auto height = HGE::randomNumberBetween<int>(sMinimumRoomSize, sMaximumRoomSize);

        rooms.emplace_back(std::make_unique<Room>(
                Room(i, HGE::Recti(randomPointInCircle(), { width, height }))));
    }
    return std::move(rooms);
}

/**
 *
 * @param rooms
 * @return
 */
auto RoomGenerator::extractSmallAreaRoomsFromVector(std::vector<std::unique_ptr<Room>> &rooms)
        -> std::vector<std::unique_ptr<Room>> {

    // todo: minimum edge length discarded?
    // todo: Expose as public variable
    const auto isAreaSmall = [](const auto &room) {
        return room->mRect.area() < sMinimumRoomArea;
    };

    // todo: make some sort of vector to vector algorithm? move if?
    auto unusedRooms = std::vector<std::unique_ptr<Room>>();
    std::for_each(rooms.rbegin(), rooms.rend(), [&isAreaSmall, &rooms, &unusedRooms](auto & room) {
        if(isAreaSmall(room)) {
            auto it = std::find(rooms.begin(), rooms.end(), room);
            unusedRooms.push_back(std::move(*it));
            rooms.erase(it);
        }
    });
    return unusedRooms;
}