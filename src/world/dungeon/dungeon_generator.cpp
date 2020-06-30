//
// Created by David Price on 26/05/2020.
//

#include "world/dungeon/dungeon_generator.h"

#include <algorithm>
#include <athena_functions.h>
#include <functional>
#include <vector>

#include "world/dungeon/path_generator.h"
#include "world/dungeon/room_generator.h"

#include "maths/delaunay.h"
#include "maths/minimum_spanning_tree.h"

/**
 *
 * @param rooms
 * @return
 */

auto DungeonGenerator::createDungeonGridFromRooms(std::vector<std::unique_ptr<Room>> &rooms)
-> HGE::Grid<std::unique_ptr<GridTile>> {
    const auto highestRoom = [](const auto &a, const auto &b) { return a->mRect.position().y < b->mRect.position().y; };
    const auto leftMostRoom = [](const auto &a, const auto &b) { return a->mRect.position().x > b->mRect.position().x; };
    const auto rightMostRoom = [](const auto &a, const auto &b) { return a->mRect.position().x < b->mRect.position().x; };
    const auto lowestRoom = [](const auto &a, const auto &b) { return a->mRect.position().y > b->mRect.position().y; };

    auto top = std::max_element(rooms.begin(), rooms.end(), highestRoom)->get()->mRect.topLeft().y;
    auto left = std::max_element(rooms.begin(), rooms.end(), leftMostRoom)->get()->mRect.topLeft().x;
    auto right = std::max_element(rooms.begin(), rooms.end(), rightMostRoom)->get()->mRect.topRight().x;
    auto bottom = std::max_element(rooms.begin(), rooms.end(), lowestRoom)->get()->mRect.bottomLeft().y;

    auto width = right - left + (sGridSpacing * 2);
    auto height = top - bottom + (sGridSpacing * 2);
    auto originX = left - sGridSpacing;
    auto originY = bottom - sGridSpacing;

    auto newGrid = HGE::Grid<std::unique_ptr<GridTile>>(width, height, originX, originY);
    return std::move(newGrid);
}

/** Insert a vector of rooms into the dungeon grid. */
void DungeonGenerator::insertRoomsIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid,
                                           std::vector<std::unique_ptr<Room>> &rooms) {
    for (const auto &room : rooms) {
        insertRoomIntoGrid(grid, room.get());
    }
}

/**
 * Insert Room Into Grid
 * @param grid - grid to insert room into.
 * @param room - pointer to room to be attached to tile.
 */
void DungeonGenerator::insertRoomIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid, Room *room) {
    auto tileType = GridTileType::ROOM_FLOOR;
    const auto insertTile = [&room, &tileType]() {
        return std::make_unique<GridTile>(tileType, room);
    };

    auto &rect = room->mRect;
    auto gridView = HGE::GridView<std::unique_ptr<GridTile>>(grid, rect.bottomLeft(), rect.topRight());
    std::generate(gridView.begin(), gridView.end(), insertTile);

    tileType = GridTileType::WALL;
    std::generate(grid.row(rect.bottomLeft()), ++grid.row(rect.bottomRight()), insertTile);
    std::generate(grid.row(rect.topLeft()), ++grid.row(rect.topRight()), insertTile);
    std::generate(++grid.column(rect.bottomLeft()), grid.column(rect.topLeft()), insertTile);
    std::generate(++grid.column(rect.bottomRight()), grid.column(rect.topRight()), insertTile);
}

/**
 *
 */
void insertPathIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid, Path &path) {
    const static auto surroundingOffset = std::array<HGE::Vector2i, 8> {
            HGE::Vector2i{ 0 , 1 },
            HGE::Vector2i{ 1 , 1 },
            HGE::Vector2i{ 1 , 0 },
            HGE::Vector2i{ 1 , -1 },
            HGE::Vector2i{ 0 , -1 },
            HGE::Vector2i{ -1 , -1 },
            HGE::Vector2i{ -1 , 0 },
            HGE::Vector2i{ -1 , 1 }
    };

    const auto addPathToGrid = [&grid, &path] (const auto & node) {
        const auto setSurroundingWalls = [&grid, &node, &path] (const auto & offset) {
            if(grid.isPointInGrid(node + offset)) {
                GridTile *tilePtr = grid.at(node + offset).get();
                if (tilePtr == nullptr) {
                    grid.at(node + offset) = std::make_unique<GridTile>(GridTileType::WALL);
                } else if (tilePtr->mType != GridTileType::WALL
                           && tilePtr->mRoomPtr != nullptr
                           && (tilePtr->mRoomPtr->mId == path.mConnectedRooms.at(0)
                               || tilePtr->mRoomPtr->mId == path.mConnectedRooms.at(1))) {
                    tilePtr->mType = GridTileType::DOOR;
                } else if (tilePtr->mType != GridTileType::PATH
                           && tilePtr->mType != GridTileType::DOOR
                           && tilePtr->mType != GridTileType::ROOM_FLOOR) {
                    tilePtr->mType = GridTileType::WALL;
                }
            }
        };

        std::for_each(surroundingOffset.begin(), surroundingOffset.end(), setSurroundingWalls);
        grid.at(node)->mType = GridTileType::PATH;
    };
    std::for_each(path.mNodes.begin(), path.mNodes.end(), addPathToGrid);
}

/**
 *
 */
auto DungeonGenerator::generate() -> Dungeon {

    auto roomGenerator = RoomGenerator(mGenerator);
    std::vector<std::unique_ptr<Room>> unusedRooms{};
    auto generatedRooms = roomGenerator.generateRooms(sNumberOfInitialRooms);

    auto const midpointFromRoom = [](const auto &room) {
        return std::make_pair(room->mId, HGE::Vector2f(room->mRect.midpoint()));
    };

    auto midpoints = std::vector<std::pair<int, HGE::Vector2f>>();
    std::transform(generatedRooms.mRooms.begin(),
                   generatedRooms.mRooms.end(),
                   std::back_inserter(midpoints),
                   midpointFromRoom);

    auto triangulation = Delaunay::triangulationFromPoints(midpoints);
    auto minimumSpanTree = MinimumSpanningTree::treeFromDelaunayTriangulation(triangulation);

    auto grid = createDungeonGridFromRooms(generatedRooms.mRooms);
    insertRoomsIntoGrid(grid, generatedRooms.mRooms);

    const auto generatePathFromConnection = [&grid, &generatedRooms] (const auto & connection) {
        auto path = PathGenerator::generatePath(grid, generatedRooms.mRooms, connection);
        insertPathIntoGrid(grid, path);
        return path;
    };

    auto paths = std::vector<Path>{ };
    std::transform(minimumSpanTree.mConnections.begin(),
                   minimumSpanTree.mConnections.end(),
                   std::back_inserter(paths),
                   generatePathFromConnection);

    for (const auto & path : paths) {
        for (int i = 1; i < path.mNodes.size(); ++i) {
            auto nodeA = path.mNodes.at(i - 1);
            auto nodeB = path.mNodes.at(i);

            mDebug->drawLine(HGE::Vector2f(400 + nodeA.x, 300 + nodeA.y),
                             HGE::Vector2f(400 + nodeB.x, 300 + nodeB.y),
                             10.0f,
                             {255, 255, 255});
        }
    }

    /* get all rooms that overlap a path */

    /* todo: extract to function */
    auto overlappingRooms = std::vector<std::unique_ptr<Room>>{};

    std::for_each(paths.begin(), paths.end(), [&generatedRooms, &overlappingRooms](const Path &path) {
      std::for_each(path.mNodes.begin(), path.mNodes.end(), [&generatedRooms, &overlappingRooms](const auto & node) {

            const auto doesPathPointerOverlap = [&node](const auto &room) {
                return room->mRect.containsPoint(node);
            };

            auto roomIt = std::find_if(generatedRooms.mUnusedRooms.begin(),
                                       generatedRooms.mUnusedRooms.end(),
                                       doesPathPointerOverlap);


            while (roomIt != generatedRooms.mUnusedRooms.end()) {
                overlappingRooms.push_back(std::move(*roomIt));
                generatedRooms.mUnusedRooms.erase(roomIt);

                roomIt = std::find_if(generatedRooms.mUnusedRooms.begin(),
                                      generatedRooms.mUnusedRooms.end(),
                                      doesPathPointerOverlap);
            }
        });
    });

    /* add these rooms, updating the path */


    /* todo: return dungeon */
    auto dungeon = Dungeon();
    dungeon.mMainRooms = std::move(generatedRooms.mRooms);
    dungeon.mMinorRooms = std::move(overlappingRooms);
    return std::move(dungeon);
}