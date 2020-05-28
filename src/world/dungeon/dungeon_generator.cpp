//
// Created by David Price on 26/05/2020.
//

#include "world/dungeon/dungeon_generator.h"

#include <cmath>
#include <vector>
#include <algorithm>

#include <maths/maths_types.h>

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
    const auto highestRoom = [](const auto &a, const auto &b) { return a->mRect.mPosition.y < b->mRect.mPosition.y; };
    const auto leftMostRoom = [](const auto &a, const auto &b) { return a->mRect.mPosition.x > b->mRect.mPosition.x; };
    const auto rightMostRoom = [](const auto &a, const auto &b) { return a->mRect.mPosition.x < b->mRect.mPosition.x; };
    const auto lowestRoom = [](const auto &a, const auto &b) { return a->mRect.mPosition.y > b->mRect.mPosition.y; };

    auto top = std::max_element(rooms.begin(), rooms.end(), highestRoom)->get()->mRect.topLeft().y;
    auto left = std::max_element(rooms.begin(), rooms.end(), leftMostRoom)->get()->mRect.topLeft().x;
    auto right = std::max_element(rooms.begin(), rooms.end(), rightMostRoom)->get()->mRect.topRight().x;
    auto bottom = std::max_element(rooms.begin(), rooms.end(), lowestRoom)->get()->mRect.bottomLeft().y;

    auto width = right - left + (sGridSpacing * 2);
    auto height = top - bottom + (sGridSpacing * 2);
    auto originX = left - sGridSpacing;
    auto originY = bottom - sGridSpacing;

    /* todo: pointer issue? grid size? */
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
 *
 * @param grid
 * @param room
 */
// todo: heavy duplication, can reduce?
void DungeonGenerator::insertRoomIntoGrid(HGE::Grid<std::unique_ptr<GridTile>> &grid, Room *room) {
    /* insert bottom row of walls */
    for (int i = 0; i < room->mRect.mSize.x; ++i) {
        grid.at(room->mRect.mPosition.x + i, room->mRect.mPosition.y)
                = std::make_unique<GridTile>(GridTileType::WALL, room);
    }

    /* insert top row of walls */
    for (int i = 0; i < room->mRect.mSize.x; ++i) {
        grid.at(room->mRect.mPosition.x + i, room->mRect.topLeft().y)
                = std::make_unique<GridTile>(GridTileType::WALL, room);
    }

    /* insert left row of walls */
    for (int i = 0; i < room->mRect.mSize.y; ++i) {
        grid.at(room->mRect.mPosition.x, room->mRect.mPosition.y + i)
                = std::make_unique<GridTile>(GridTileType::WALL, room);
    }

    /* insert right row of walls */
    for (int i = 0; i < room->mRect.mSize.y; ++i) {
        grid.at(room->mRect.bottomRight().x, room->mRect.mSize.y + i)
                = std::make_unique<GridTile>(GridTileType::WALL, room);
    }

    /* insert floor tiles */
    for (int i = 1; i < room->mRect.mSize.y - 1; ++i) {
        for (int i = 1; i < room->mRect.mSize.x - 1; ++i) {
            grid.at(room->mRect.mPosition.x + i, room->mRect.mSize.y + i)
                    = std::make_unique<GridTile>(GridTileType::ROOM_FLOOR, room);
        }
    }
}

/**
 *
 */
auto DungeonGenerator::generate() -> Dungeon {

    auto roomGenerator = RoomGenerator(mGenerator);
    std::vector<std::unique_ptr<Room>> unusedRooms{ };
    auto generatedRooms = roomGenerator.generateRooms(sNumberOfInitialRooms);

    auto const midpointFromRoom = [](const auto &room) {
        return std::make_pair(room->mId, room->mRect.midpoint());
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

    for (auto & connection : minimumSpanTree.mConnections) {
        auto path = PathGenerator::generatePath(grid, generatedRooms.mRooms, connection);

        for (int i = 1; i < path.mNodes.size(); ++i) {
            auto nodeA = path.mNodes.at(i - 1);
            auto nodeB = path.mNodes.at(i);

            mDebug->drawLine(HGE::Vector2f(400 + nodeA.x, 300 + nodeA.y),
                             HGE::Vector2f(400 + nodeB.x, 300 + nodeB.y),
                             10.0f,
                             { 255, 255, 255 });
        }

        /** todo: remove debug draw! */
        for (auto const &connection : minimumSpanTree.mConnections) {
            auto a = std::find_if(triangulation.mVertices.begin(), triangulation.mVertices.end(),
                                  [&connection](std::unique_ptr<Vertex> &vert) {
                                      return vert->mId == connection.mA;
                                  })->get();

            auto b = std::find_if(triangulation.mVertices.begin(), triangulation.mVertices.end(),
                                  [&connection](std::unique_ptr<Vertex> &vert) {
                                      return vert->mId == connection.mB;
                                  })->get();

            mDebug->drawLine(HGE::Vector2f(400 + a->x(), 300 + a->y()),
                             HGE::Vector2f(400 + b->x(), 300 + b->y()),
                             10.0f,
                             { 255, 0, 255 });
        }
    }

    /* Get position of the door for each room */

    /* for each connection, pass the map grid and connection a pathing class */

    /* check if any rooms intersect the path */

    /* insert minor rooms that intersect the path */

    /* update path and insert that into the map grid */

    /* todo: return dungeon */
    auto dungeon = Dungeon();
    dungeon.mMainRooms = std::move(generatedRooms.mRooms);
    return std::move(dungeon);
}