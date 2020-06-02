//
// Created by David Price on 28/05/2020.
//

#include "world/dungeon/path_generator.h"

#include <cmath>
#include <maths/maths_types.h>

#include "world/dungeon/breadth_path_generator.h"

/**
 *
 * @param room
 * @return
 * todo: adjust angles to be midpoint to room corner angles
 */
auto PathGenerator::pointOnRoomClosestToPoint(Room *room, HGE::Vector2i point) -> HGE::Vector2i {

    HGE::Vector2i difference = point - HGE::Vector2i(room->mRect.midpoint());
    auto angle = atan2(difference.y, difference.x) * (180 / M_PI);

    if ((angle >= -45 && angle <= 0) || (angle >= 0 && angle <= 45)) {
        auto randomRightSide = HGE::randomNumberBetween(
                room->mRect.bottomRight().y + 1,
                room->mRect.topRight().y - 1);
        return { room->mRect.topRight().x, randomRightSide };
    }

    if ((angle <= -135 && angle >= -180) || (angle <= 180 && angle >= 135)) {
        auto randomLeftSide = HGE::randomNumberBetween(
                room->mRect.bottomLeft().y + 1,
                room->mRect.topLeft().y - 1);
        return { room->mRect.topLeft().x, randomLeftSide };
    }

    if (angle >= 45 && angle <= 135) {
        auto randomTopSide = HGE::randomNumberBetween(
                room->mRect.topLeft().x + 1,
                room->mRect.topRight().x - 1);
        return { randomTopSide, room->mRect.topRight().y };
    }

    auto randomBottomSide = HGE::randomNumberBetween(
            room->mRect.bottomLeft().x + 1,
            room->mRect.bottomRight().x - 1);
    return { randomBottomSide, room->mRect.bottomRight().y };
}

/**
 *
 * @param grid
 * @param rooms
 * @param connection
 * @return
 */
auto PathGenerator::generatePath(HGE::Grid<std::unique_ptr<GridTile> > &grid,
                                 const std::vector<std::unique_ptr<Room> > &rooms,
                                 const Connection &connection) -> Path {

    auto roomA = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mA == room->mId;
    });

    auto roomB = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mB == room->mId;
    });

    auto roomIds = std::vector<int>{ roomA->get()->mId, roomB->get()->mId };

    constexpr auto scoreByTile = [](const auto &tile, const auto &score) {
        if (tile == nullptr) {
            return score + 1;
        }
        switch (tile->mType) {
            case GridTileType::ROOM_FLOOR:
            case GridTileType::WALL:
                return 99999999;
            default:
                return score + 1;
        }
    };

    const auto straightPathing = [] (const auto & adjacent) {

        constexpr auto lowestScore = [](const auto &a, const auto &b) {
            return a.mScore < b.mScore;
        };

        return std::min_element(adjacent.begin(), adjacent.end(), lowestScore);
    };

    auto start = pointOnRoomClosestToPoint(roomA->get(), HGE::Vector2i(roomB->get()->mRect.midpoint()));
    auto finish = pointOnRoomClosestToPoint(roomB->get(), start);

    return BreadthPathGenerator(grid, scoreByTile, straightPathing).generatePath(start, finish, roomIds);
}