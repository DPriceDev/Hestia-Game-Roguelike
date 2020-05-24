//
// Created by David Price on 15/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
#define HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H

#include <cmath>
#include <vector>
#include <random>
#include <algorithm>

#include "maths/maths_types.h"
#include <util/logger.h>
#include <framework/systems/debug_system.h>

#include "maths/delaunay.h"
#include "maths/minimum_spanning_tree.h"
#include "room.h"
#include "path.h"
#include "dungeon_grid.h"
#include "path_generator.h"

struct Dungeon {
    std::vector<Room> mMainRooms;
    std::vector<Room> mMinorRooms;
    std::vector<Path> mPaths;
};

/**
 * Dungeon Generator
 */
class DungeonGenerator {
    const static int sNumberOfInitialRooms = 100;
    constexpr static float S_RADIUS = 100.0f;
    constexpr static float S_RADIUS_SQRD = S_RADIUS * S_RADIUS;
    constexpr static float S_TWO_PI = 2 * M_PI;
    constexpr static float sMinimumRoomSize = 6.0f;
    constexpr static float sMaximumRoomSize = 20.0f;
    constexpr static float sMinimumRoomArea = 220.0f;
    constexpr static float sSeperationFactor = 4.0f;
    const static int sGridSpacing = 5;

    HGE::DebugComponent* mDebug;

    std::vector<std::unique_ptr<Room>> mRooms{ };
    std::random_device mRandomDevice{ };
    std::mt19937 mGenerator{ mRandomDevice() };
    std::uniform_real_distribution<float> mRadiusDistribution { 0, S_RADIUS_SQRD };
    std::uniform_real_distribution<float> mAngleDistribution { 0, S_TWO_PI };

    /** Get a random point in a circle */
    HGE::Vector2f randomPointInCircle() {
        float radius = std::sqrt(mRadiusDistribution(mGenerator));
        float angle = mAngleDistribution(mGenerator);
        return { HGE::roundValueToMultipleOf(radius * cos(angle), 1.0f),
                 HGE::roundValueToMultipleOf(radius * sin(angle), 1.0f)};
    }

    /** Separate a set of rooms so that none overlap */
    static auto separateRooms(std::vector<std::unique_ptr<Room>> &rooms) {

        std::vector<Room*> pRooms;
        std::transform(rooms.begin(), rooms.end(), std::back_inserter(pRooms), [] (const auto & room) {
           return room.get();
        });

        bool overlapsExist = true;
        while(overlapsExist) {
            overlapsExist = false;

            std::for_each(pRooms.begin(), pRooms.end(), [&] (auto & room) {
                std::vector<Room*> overlapping { }, overlappingSame { };

                std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlapping), [&] (auto & o) {
                    return room->mRect.isOverlapping(o->mRect)
                        && room != o
                        && room->mRect.midpoint() != o->mRect.midpoint();
                });

                std::copy_if(pRooms.begin(), pRooms.end(), std::back_inserter(overlappingSame), [&] (auto & o) {
                    return room->mRect.isOverlapping(o->mRect)
                        && room != o
                        && room->mRect.midpoint() == o->mRect.midpoint();
                });

                const auto matchSeparate = [&] (auto & a, auto & b) {
                    overlapsExist = true;
                    return a + (room->mRect.midpoint() - b->mRect.midpoint()) / sSeperationFactor;
                };

                const auto matchSame = [&] (auto & a, auto & b) {
                    overlapsExist = true;
                    return a + room->mRect.midpoint().normalised();
                };

                room->mMovement += std::accumulate(overlapping.begin(), overlapping.end(), HGE::Vector2f(),matchSeparate);
                room->mMovement += std::accumulate(overlappingSame.begin(), overlappingSame.end(), HGE::Vector2f(),matchSame);
            });

            std::for_each(pRooms.begin(), pRooms.end(), [] (auto & room) {
                room->mMovement.x = HGE::roundValueToMultipleOf(room->mMovement.x, 1.0f);
                room->mMovement.y = HGE::roundValueToMultipleOf(room->mMovement.y, 1.0f);
                room->mRect.mPosition += room->mMovement;
                room->mMovement = HGE::Vector2f();
            });
        }

        return std::move(rooms);
    }

    /** Generate a set of random rooms at random locations */
    std::vector<std::unique_ptr<Room>> generateRandomRooms(const int numberOfRooms) {
        std::vector<std::unique_ptr<Room>> rooms{ };

        for(int i = 0; i < numberOfRooms; ++i) {
            auto width = HGE::roundValueToMultipleOf(
                    HGE::randomNumberBetween<float>(sMinimumRoomSize, sMaximumRoomSize), 1.0f);

            auto height = HGE::roundValueToMultipleOf(
                    HGE::randomNumberBetween<float>(sMinimumRoomSize, sMaximumRoomSize), 1.0f);

            rooms.emplace_back(std::make_unique<Room>(Room(i, HGE::Rectf(randomPointInCircle(), { width, height }))));
        }
        return std::move(rooms);
    }

    /** create a dungeon grid that will fit all the rooms */
    static DungeonGrid createDungeonGridFromRooms(std::vector<std::unique_ptr<Room>>& rooms) {
        const auto highestRoom = [] (const auto & a, const auto & b) { return a->mRect.mPosition.y < b->mRect.mPosition.y; };
        const auto leftMostRoom = [] (const auto & a, const auto & b) { return a->mRect.mPosition.x > b->mRect.mPosition.x; };
        const auto rightMostRoom = [] (const auto & a, const auto & b) { return a->mRect.mPosition.x < b->mRect.mPosition.x; };
        const auto lowestRoom = [] (const auto & a, const auto & b) { return a->mRect.mPosition.y > b->mRect.mPosition.y; };

        auto top = std::max_element(rooms.begin(), rooms.end(), highestRoom);
        auto left = std::max_element(rooms.begin(), rooms.end(), leftMostRoom);
        auto right = std::max_element(rooms.begin(), rooms.end(), rightMostRoom);
        auto bottom = std::max_element(rooms.begin(), rooms.end(), lowestRoom);

        auto grid = DungeonGrid(top->get()->mRect.topLeft().y + sGridSpacing,
                                right->get()->mRect.topRight().x + sGridSpacing,
                                left->get()->mRect.topLeft().x - sGridSpacing,
                                bottom->get()->mRect.bottomLeft().y - sGridSpacing);
        return std::move(grid);
    }

    /** Insert a vector of rooms into the dungeon grid. */
    static void insertRoomsIntoGrid(DungeonGrid &grid, std::vector<std::unique_ptr<Room>>& rooms) {
        for(const auto & room : rooms) {
            insertRoomIntoGrid(grid, room.get());
        }
    }

    /** inserts a single room into the supplied grid */
    // todo: heavy duplication, can reduce?
    static void insertRoomIntoGrid(DungeonGrid &grid, Room* room) {
        /* insert bottom row of walls */
        for(int i = 0; i < room->mRect.mSize.x; ++i) {
            auto gridPosition = grid[room->mRect.mPosition.y][room->mRect.mPosition.x + i];
            gridPosition->mType = GridTileType::WALL;
            gridPosition->mRoomPtr = room;
        }

        /* insert top row of walls */
        for(int i = 0; i < room->mRect.mSize.x; ++i) {
            auto gridPosition = grid[room->mRect.topLeft().y][room->mRect.mPosition.x + i];
            gridPosition->mType = GridTileType::WALL;
            gridPosition->mRoomPtr = room;
        }

        /* insert left row of walls */
        for(int i = 0; i < room->mRect.mSize.y; ++i) {
            auto gridPosition = grid[room->mRect.mPosition.y + i][room->mRect.mPosition.x];
            gridPosition->mType = GridTileType::WALL;
            gridPosition->mRoomPtr = room;
        }

        /* insert right row of walls */
        for(int i = 0; i < room->mRect.mSize.y; ++i) {
            auto gridPosition = grid[room->mRect.mSize.y + i][room->mRect.bottomRight().x];
            gridPosition->mType = GridTileType::WALL;
            gridPosition->mRoomPtr = room;
        }

        /* insert floor tiles */
        for(int i = 1; i < room->mRect.mSize.y-1; ++i) {
            for(int i = 1; i < room->mRect.mSize.x - 1; ++i) {
                auto gridPosition = grid[room->mRect.mSize.y + i][room->mRect.mPosition.x + i];
                gridPosition->mType = GridTileType::ROOM_FLOOR;
                gridPosition->mRoomPtr = room;
            }
        }
    }

    static auto extractSmallAreaRoomsFromVector(std::vector<std::unique_ptr<Room>> &rooms) {
        const auto isAreaSmall = [] (const auto & room) {
            return room->mRect.area() < sMinimumRoomArea;
        };

        auto unusedRooms = std::vector<std::unique_ptr<Room>>();
        auto unusedIt = std::remove_if(rooms.begin(), rooms.end(), isAreaSmall);
        std::move(unusedIt, rooms.end(), std::back_inserter(unusedRooms));
        rooms.erase(unusedIt, rooms.end());

        return std::move(unusedRooms);
    }

public:
    /** todo: replace with a static method? */
    explicit DungeonGenerator(HGE::DebugComponent* debug) : mDebug(debug) { }
    ~DungeonGenerator() = default;

    /** returns the room list */
    const std::vector<std::unique_ptr<Room>>& getRooms() {
        return mRooms;
    }

    /** generate the dungeon */
    void generate() {
        mRooms = generateRandomRooms(sNumberOfInitialRooms);
        mRooms = separateRooms(mRooms);
        auto unusedRooms = extractSmallAreaRoomsFromVector(mRooms);

        /* vector of midpoints */
        auto const midpointFromRoom = [] (const auto & room) {
            return std::make_pair(room->mId, room->mRect.midpoint());
        };

        auto midpoints = std::vector<std::pair<int, HGE::Vector2f>>();
        std::transform(mRooms.begin(), mRooms.end(), std::back_inserter(midpoints), midpointFromRoom);

        auto triangulation = delaunayTriangulationFromPoints(midpoints);
        auto minimumSpanTree = minimumSpanningTreeFromDelaunayTriangulation(triangulation);

        auto grid = createDungeonGridFromRooms(mRooms);
        insertRoomsIntoGrid(grid, mRooms);

        /* todo refactor out as a method */
        /* generate paths between rooms */
        for(int c = 0; c < 5; ++c) {

            const auto & connection = minimumSpanTree.mConnections.at(c);

            /* get connected rooms */
            auto roomA = std::find_if(mRooms.begin(), mRooms.end(), [&] (const auto & room) {
                return connection.mA == room->mId;
            });

            auto roomB = std::find_if(mRooms.begin(), mRooms.end(), [&] (const auto & room) {
                return connection.mB == room->mId;
            });

            /* Get position of the door for each room */

            generatePath(grid, roomA->get(), roomB->get());

            /* for each connection, pass the map grid and connection a pathing class */

            /* check if any rooms intersect the path */

            /* insert minor rooms that intersect the path */

            /* update path and insert that into the map grid */

            auto path = generatePath(grid, roomA->get(), roomB->get());


            for(int i = 1; i < path.mNodes.size(); ++i) {
                auto nodeA = path.mNodes.at(i - 1);
                auto nodeB = path.mNodes.at(i);

                mDebug->drawLine(HGE::Vector2f ( 400 + nodeA.x, 300 + nodeA.y ),
                                 HGE::Vector2f ( 400 + nodeB.x, 300 + nodeB.y ),
                                 10.0f,
                                 {255, 255, 255} );
            }

        }

        /** todo: remove debug draw! */
        for(auto const & connection : minimumSpanTree.mConnections) {
            auto a = std::find_if(triangulation.mVertices.begin(), triangulation.mVertices.end(),
                                  [&connection] (std::unique_ptr<Vertex> &vert) {
                                      return vert->mId == connection.mA;
                                  })->get();

            auto b = std::find_if(triangulation.mVertices.begin(), triangulation.mVertices.end(),
                                  [&connection] (std::unique_ptr<Vertex> &vert) {
                                      return vert->mId == connection.mB;
                                  })->get();

            mDebug->drawLine(HGE::Vector2f ( 400 + a->x(), 300 + a->y() ),
                             HGE::Vector2f ( 400 + b->x(), 300 + b->y() ),
                             10.0f,
                             {255, 0, 255} );
        }

        /* todo: return dungeon */
    }
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
