//
// Created by David Price on 28/05/2020.
//

#include "world/dungeon/path_generator.h"
#include <maths/maths.h>
#include <optional>
#include <unordered_map>

#include "world/dungeon/breadth_path_generator.h"

/* */
static const std::map<HGE::Vector2i, int> sOffsetMap{
        std::make_pair(HGE::Vector2i(0, 0), 0),
        std::make_pair(HGE::Vector2i(0, 1), 2),
        std::make_pair(HGE::Vector2i(1, 0), 3),
        std::make_pair(HGE::Vector2i(0, -1), 0),
        std::make_pair(HGE::Vector2i(-1, 0), 1)};

static constexpr auto lowestScore = [](const auto &a, const auto &b) {
    return a.mScore < b.mScore;
};

/* */
static constexpr auto getNextTileWithStraightPathing = [](const auto &pathingData) {

    auto lowestTile = std::min_element(pathingData.mAdjacentTiles.begin(),
                                       pathingData.mAdjacentTiles.end(),
                                       lowestScore);

    auto diff = pathingData.mCurrentTile - pathingData.mPreviousTile;
    auto tile = pathingData.mAdjacentTiles.at(sOffsetMap.at(diff));
    if (tile.mScore == lowestTile->mScore && diff != HGE::Vector2i(0, 0)) {
        return tile;
    } else {
        return *lowestTile;
    }
};


/* */
Path PathGenerator::shrinkPathToRoomWalls(HGE::Grid<std::unique_ptr<GridTile>> &grid, Path &path,
                                          Room *roomA, Room *roomB) {
    auto room = roomB;
    const auto doesNodeMatchesRoomWall = [&grid, &room](const HGE::Vector2i &node) {
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

/* */
auto PathGenerator::extractRoomPointersFromVectorById(const std::vector<std::unique_ptr<Room>> &rooms,
                                                      const Connection &connection) -> std::pair<Room *, Room *> {
    auto node = connection.mA;
    const auto findRoomById = [&node](const auto &room) {
        return node == room->mId;
    };

    auto roomA = std::find_if(rooms.begin(), rooms.end(), findRoomById);
    node = connection.mB;
    auto roomB = std::find_if(rooms.begin(), rooms.end(), findRoomById);
    return std::make_pair(roomA->get(), roomB->get());
}

/* */
Path PathGenerator::generatePath(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                 const std::vector<std::unique_ptr<Room>> &rooms,
                                 const Connection &connection) {
    Room *startRoom;
    Room *finishRoom;
    std::tie(startRoom, finishRoom) = extractRoomPointersFromVectorById(rooms, connection);

    const auto getNextScoreFromForTile = [&startRoom, &finishRoom](const auto &position, auto &tile, const auto &score) {
        if (tile == nullptr) {
            return score + 1;
        } else {
            switch (tile->mType) {
                case GridTileType::DOOR:
                    return 99999999;
                case GridTileType::ROOM_FLOOR:
                case GridTileType::WALL:
                case GridTileType::PATH:
                    // todo: ignore corners
                    if (tile->mRoomPtr == startRoom || tile->mRoomPtr == finishRoom) {
                        return score + 1;
                    } else {
                        return 99999999;
                    }
                default:
                    return score + 1;
            }
        }
    };

    auto roomIds = std::vector<int>{startRoom->mId, finishRoom->mId};
    auto path = BreadthPathGenerator(grid, getNextScoreFromForTile, getNextTileWithStraightPathing)
                        .generatePath(HGE::Vector2i(startRoom->mRect.midpoint()),
                                      HGE::Vector2i(finishRoom->mRect.midpoint()),
                                      roomIds);

    return shrinkPathToRoomWalls(grid, path, startRoom, finishRoom);
}