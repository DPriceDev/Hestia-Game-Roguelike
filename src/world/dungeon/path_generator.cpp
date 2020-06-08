//
// Created by David Price on 28/05/2020.
//

#define _USE_MATH_DEFINES

#include "world/dungeon/path_generator.h"
#include <cmath>
#include <unordered_map>

#include "world/dungeon/breadth_path_generator.h"

/**
 *
 * @param room
 * @return
 * todo: adjust angles to be midpoint to room corner angles
 */
auto PathGenerator::pointOnRoomClosestToPoint(Room *room, HGE::Vector2i point) -> HGE::Vector2i {
    static const float degreesMultiplier = 180 / M_PI;

    HGE::Vector2i difference = point - HGE::Vector2i(room->mRect.midpoint());
    auto angle = atan2(difference.y, difference.x) * degreesMultiplier;

    if ((angle >= -45 && angle <= 0) || (angle >= 0 && angle <= 45)) {
        auto randomRightSide = HGE::randomNumberBetween(
                room->mRect.bottomRight().y + 1,
                room->mRect.topRight().y - 1);
        return {room->mRect.topRight().x, randomRightSide};
    }

    if ((angle <= -135 && angle >= -180) || (angle <= 180 && angle >= 135)) {
        auto randomLeftSide = HGE::randomNumberBetween(
                room->mRect.bottomLeft().y + 1,
                room->mRect.topLeft().y - 1);
        return {room->mRect.topLeft().x, randomLeftSide};
    }

    if (angle >= 45 && angle <= 135) {
        auto randomTopSide = HGE::randomNumberBetween(
                room->mRect.topLeft().x + 1,
                room->mRect.topRight().x - 1);
        return {randomTopSide, room->mRect.topRight().y};
    }

    auto randomBottomSide = HGE::randomNumberBetween(
            room->mRect.bottomLeft().x + 1,
            room->mRect.bottomRight().x - 1);
    return {randomBottomSide, room->mRect.bottomRight().y};
}


static const std::map<HGE::Vector2i, int> sOffsetMap {
        std::make_pair(HGE::Vector2i(0,0), 0),
        std::make_pair(HGE::Vector2i(0,1), 2),
        std::make_pair(HGE::Vector2i(1,0), 3),
        std::make_pair(HGE::Vector2i(0,-1), 0),
        std::make_pair(HGE::Vector2i(-1,0), 1) };

/**
 *
 * @param grid
 * @param rooms
 * @param connection
 * @return
 */
auto PathGenerator::generatePath(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                 const std::vector<std::unique_ptr<Room>> &rooms,
                                 const Connection &connection) -> Path {

    auto roomA = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mA == room->mId;
    });

    auto roomB = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mB == room->mId;
    });

    auto roomIds = std::vector<int>{roomA->get()->mId, roomB->get()->mId};

    // todo: refactor to get aligned point if the rooms align correctly.
    auto start = pointOnRoomClosestToPoint(roomA->get(), HGE::Vector2i(roomB->get()->mRect.midpoint()));
    auto finish = pointOnRoomClosestToPoint(roomB->get(), start);

    const auto scoreByTile = [&finish](const auto &position, const auto &tile, const auto &score) {
        if (tile == nullptr) {
            return score + 1;
        }
        switch (tile->mType) {
            case GridTileType::ROOM_FLOOR:
            case GridTileType::WALL:
                if(position == finish) {
                    return score + 1;
                } else {
                    return 99999999;
                }
            default:
                return score + 1;
        }
    };

    constexpr auto straightPathing = [](const auto &pathingData) {
        constexpr auto lowestScore = [](const auto &a, const auto &b) {
            return a.mScore < b.mScore;
        };

        auto lowestTile = std::min_element(pathingData.mAdjacentTiles.begin(),
                                           pathingData.mAdjacentTiles.end(),
                                           lowestScore);

        auto diff = pathingData.mCurrentTile - pathingData.mPreviousTile;
        auto tile = pathingData.mAdjacentTiles.at(sOffsetMap.at(diff));
        if(tile.mScore == lowestTile->mScore && diff != HGE::Vector2i(0,0)) {
            return tile;
        } else {
            return *lowestTile;
        }
    };

    return BreadthPathGenerator(grid, scoreByTile, straightPathing).generatePath(start, finish, roomIds);
}