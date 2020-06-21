//
// Created by David Price on 28/05/2020.
//

#include "world/dungeon/path_generator.h"
#include <unordered_map>
#include <maths/maths.h>
#include <optional>

#include "world/dungeon/breadth_path_generator.h"

static const std::map<HGE::Vector2i, int> sOffsetMap {
        std::make_pair(HGE::Vector2i(0,0), 0),
        std::make_pair(HGE::Vector2i(0,1), 2),
        std::make_pair(HGE::Vector2i(1,0), 3),
        std::make_pair(HGE::Vector2i(0,-1), 0),
        std::make_pair(HGE::Vector2i(-1,0), 1) };

Path PathGenerator::shrinkPathToRoomWalls(HGE::Grid<std::unique_ptr<GridTile>> &grid, Path &path,
                                          Room *roomA, Room *roomB) {
    auto room = roomB;
    const auto doesNodeMatchesRoomWall = [&grid, &room] (const HGE::Vector2i & node) {
      return grid.at(node) != nullptr
               && grid.at(node)->mRoomPtr == room
               && grid.at(node)->mType == GridTileType::WALL;
    };

    auto frontIt = std::find_if(path.mNodes.begin(), path.mNodes.end(), doesNodeMatchesRoomWall);
    path.mNodes.erase(path.mNodes.begin(), frontIt - 1);

    room = roomA;
    auto backIt = std::find_if(path.mNodes.begin(), path.mNodes.end(), doesNodeMatchesRoomWall);
    path.mNodes.erase(backIt + 2, path.mNodes.end());
    return path;
}

Path PathGenerator::generatePath(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                 const std::vector<std::unique_ptr<Room>> &rooms,
                                 const Connection &connection) {

    auto roomA = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mA == room->mId;
    });

    auto roomB = std::find_if(rooms.begin(), rooms.end(), [&](const auto &room) {
        return connection.mB == room->mId;
    });

    auto roomIds = std::vector<int>{roomA->get()->mId, roomB->get()->mId};

    const auto scoreByTile = [&roomA, &roomB](const auto &position, auto &tile, const auto &score) {
        if(tile == nullptr) {
            return score + 1;
        } else {
            switch (tile->mType) {
                case GridTileType::ROOM_FLOOR:
                case GridTileType::WALL:
                case GridTileType::DOOR:
                    // todo: ignore the room corners -> function?
                    if (tile->mRoomPtr == roomA->get() || tile->mRoomPtr == roomB->get()) {
                        return score + 1;
                    } else {
                        return 99999999;
                    }
                default:
                    return score + 1;
            }
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

    auto path = BreadthPathGenerator(grid, scoreByTile, straightPathing).generatePath(HGE::Vector2i(roomA->get()->mRect.midpoint()),
                                                                                      HGE::Vector2i(roomB->get()->mRect.midpoint()),
                                                                                      roomIds);
    return shrinkPathToRoomWalls(grid, path, roomA->get(), roomB->get());
}