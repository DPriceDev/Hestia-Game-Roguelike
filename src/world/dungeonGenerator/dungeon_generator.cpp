//
// Created by David Price on 26/05/2020.
//

#include "world/dungeonGenerator/dungeon_generator.h"

#include <cmath>
#include <vector>
#include <random>
#include <algorithm>

#include <maths/maths_types.h>

#include "maths/delaunay.h"
#include "maths/minimum_spanning_tree.h"
#include "world/dungeonGenerator/breadth_path_generator.h"

/**
 *
 * @return
 */
auto DungeonGenerator::randomPointInCircle() -> HGE::Vector2i {
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
auto DungeonGenerator::separateRooms(std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>> {

    std::vector<Room *> pRooms;
    std::transform(rooms.begin(), rooms.end(), std::back_inserter(pRooms), [](const auto &room) {
        return room.get();
    });

    bool overlapsExist = true;
    while (overlapsExist) {
        overlapsExist = false;

        std::for_each(pRooms.begin(), pRooms.end(), [&](auto &room) {
            std::vector<Room *> overlapping{ }, overlappingSame{ };

            std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlapping), [&](auto &o) {
                return room->mRect.isOverlapping(o->mRect)
                       && room != o
                       && room->mRect.midpoint() != o->mRect.midpoint();
            });

            std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlappingSame), [&](auto &o) {
                return room->mRect.isOverlapping(o->mRect)
                       && room != o
                       && room->mRect.midpoint() == o->mRect.midpoint();
            });

            const auto matchSeparate = [&](auto &a, auto &b) {
                overlapsExist = true;
                auto midpoint = (room->mRect.midpoint() - b->mRect.midpoint()) / sSeperationFactor;
                midpoint.x = ceil(midpoint.x);
                midpoint.y = ceil(midpoint.y);
                return a + midpoint;
            };

            const auto matchSame = [&](auto &a, auto &b) {
                overlapsExist = true;
                auto midpointNorm = room->mRect.midpoint().normalised();
                midpointNorm.x = ceil(midpointNorm.x);
                midpointNorm.y = ceil(midpointNorm.y);
                return a + midpointNorm;
            };

            room->mMovement += std::accumulate(overlapping.begin(), overlapping.end(), HGE::Vector2i(), matchSeparate);
            room->mMovement += std::accumulate(overlappingSame.begin(), overlappingSame.end(), HGE::Vector2i(),
                                               matchSame);
        });

        std::for_each(pRooms.begin(), pRooms.end(), [](auto &room) {
            room->mRect.mPosition += room->mMovement;
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
std::vector<std::unique_ptr<Room>> DungeonGenerator::generateRandomRooms(const int numberOfRooms) {
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

auto DungeonGenerator::createDungeonGridFromRooms(
        std::vector<std::unique_ptr<Room>> &rooms) -> HGE::Grid<std::unique_ptr<GridTile>> {
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
 * @param rooms
 * @return
 */
auto DungeonGenerator::extractSmallAreaRoomsFromVector(
        std::vector<std::unique_ptr<Room>> &rooms) -> std::vector<std::unique_ptr<Room>> {
    const auto isAreaSmall = [](const auto &room) {
        return room->mRect.area() < sMinimumRoomArea;
    };

    auto unusedRooms = std::vector<std::unique_ptr<Room>>();
    auto unusedIt = std::remove_if(rooms.begin(), rooms.end(), isAreaSmall);
    std::move(unusedIt, rooms.end(), std::back_inserter(unusedRooms));
    rooms.erase(unusedIt, rooms.end());

    return std::move(unusedRooms);
}

/**
 *
 * @param room
 * @return
 * todo: adjust angles to be midpoint to room corner angles
 */
auto DungeonGenerator::pointOnRoomClosestToPoint(Room *room, HGE::Vector2i point) -> HGE::Vector2i {

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
 */
void DungeonGenerator::generate() {
    mRooms = generateRandomRooms(sNumberOfInitialRooms);
    mRooms = separateRooms(mRooms);
    auto unusedRooms = extractSmallAreaRoomsFromVector(mRooms);

    /* vector of midpoints */
    auto const midpointFromRoom = [](const auto &room) {
        return std::make_pair(room->mId, room->mRect.midpoint());
    };

    auto midpoints = std::vector<std::pair<int, HGE::Vector2f>>();
    std::transform(mRooms.begin(), mRooms.end(), std::back_inserter(midpoints), midpointFromRoom);

    auto triangulation = Delaunay::triangulationFromPoints(midpoints);
    auto minimumSpanTree = MinimumSpanningTree::treeFromDelaunayTriangulation(triangulation);

    auto grid = createDungeonGridFromRooms(mRooms);
    insertRoomsIntoGrid(grid, mRooms);

    /* todo refactor out as a method */
    /* generate paths between rooms */
    for (auto &connection : minimumSpanTree.mConnections) {

        /* get connected rooms */
        auto roomA = std::find_if(mRooms.begin(), mRooms.end(), [&](const auto &room) {
            return connection.mA == room->mId;
        });

        auto roomB = std::find_if(mRooms.begin(), mRooms.end(), [&](const auto &room) {
            return connection.mB == room->mId;
        });

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

        auto pathSearcher = BreadthPathGenerator(grid, scoreByTile);

        auto start = pointOnRoomClosestToPoint(roomA->get(), HGE::Vector2i(roomB->get()->mRect.midpoint()));
        auto finish = pointOnRoomClosestToPoint(roomB->get(), start);
        auto roomIds = std::vector<int>{ roomA->get()->mId, roomB->get()->mId };
        auto path = pathSearcher.generatePath(start, finish, roomIds);

        for (int i = 1; i < path.mNodes.size(); ++i) {
            auto nodeA = path.mNodes.at(i - 1);
            auto nodeB = path.mNodes.at(i);

            mDebug->drawLine(HGE::Vector2f(400 + nodeA.x, 300 + nodeA.y),
                             HGE::Vector2f(400 + nodeB.x, 300 + nodeB.y),
                             10.0f,
                             { 255, 255, 255 });
        }
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

    /* Get position of the door for each room */

    /* for each connection, pass the map grid and connection a pathing class */

    /* check if any rooms intersect the path */

    /* insert minor rooms that intersect the path */

    /* update path and insert that into the map grid */


    /* todo: return dungeon */
}