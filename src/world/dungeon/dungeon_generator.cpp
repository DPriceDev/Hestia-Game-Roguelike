//
// Created by David Price on 26/05/2020.
//

#include "world/dungeon/dungeon_generator.h"

#include <vector>
#include <algorithm>
#include <functional>

#include "world/dungeon/room_generator.h"
#include "world/dungeon/path_generator.h"

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

    for (auto &connection : minimumSpanTree.mConnections) {
        auto path = PathGenerator::generatePath(grid, generatedRooms.mRooms, connection);

        for (int i = 1; i < path.mNodes.size(); ++i) {
            auto nodeA = path.mNodes.at(i - 1);
            auto nodeB = path.mNodes.at(i);

            mDebug->drawLine(HGE::Vector2f(400 + nodeA.x, 300 + nodeA.y),
                             HGE::Vector2f(400 + nodeB.x, 300 + nodeB.y),
                             10.0f,
                             {255, 255, 255});
        }

        /* check if any rooms intersect the path */

        /* insert minor rooms that intersect the path */

        /* update path and insert that into the map grid */

    }

    /* todo: return dungeon */
    auto dungeon = Dungeon();
    dungeon.mMainRooms = std::move(generatedRooms.mRooms);
    return std::move(dungeon);
}