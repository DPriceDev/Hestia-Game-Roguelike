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

/**
 * Dungeon Room Struct
 */
struct Room {
    int mId;
    HGE::Rectf mRect{ };
    HGE::Vector2f mMovement{ };

    explicit Room(int id) : mId(id) { }
    Room(int id, HGE::Rectf rect) : mId(id), mRect(rect) { }
    ~Room() = default;

    bool operator!=(const Room & other) const {
        return this->mId != other.mId;
    }
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

    HGE::DebugComponent* mDebug;

    std::vector<Room> mRooms{ };
    std::random_device mRandomDevice{ };
    std::mt19937 mGenerator{ mRandomDevice() };
    std::uniform_real_distribution<float> mRadiusDistribution { 0, S_RADIUS_SQRD };
    std::uniform_real_distribution<float> mAngleDistribution { 0, S_TWO_PI };

    /* */
    HGE::Vector2f randomPointInCircle() {
        float radius = std::sqrt(mRadiusDistribution(mGenerator));
        float angle = mAngleDistribution(mGenerator);
        return { HGE::roundValueToMultipleOf(radius * cos(angle), 1.0f),
                 HGE::roundValueToMultipleOf(radius * sin(angle), 1.0f)};
    }

    /* */
    void separateRooms() {
        bool overlapsExist = true;
        while(overlapsExist) {
            overlapsExist = false;

            std::for_each(mRooms.begin(), mRooms.end(), [&] (auto & room) {
                std::vector<Room> overlapping { }, overlappingSame { };

                std::copy_if(mRooms.begin(), mRooms.end(), std::back_inserter(overlapping), [&] (auto & o) {
                    return room.mRect.isOverlapping(o.mRect)
                        && room != o
                        && room.mRect.midpoint() != o.mRect.midpoint();
                });

                std::copy_if(mRooms.begin(), mRooms.end(), std::back_inserter(overlappingSame), [&] (auto & o) {
                    return room.mRect.isOverlapping(o.mRect)
                        && room != o
                        && room.mRect.midpoint() == o.mRect.midpoint();
                });

                const auto matchSeparate = [&] (auto & a, auto & b) {
                    overlapsExist = true;
                    return a + (room.mRect.midpoint() - b.mRect.midpoint()) / sSeperationFactor;
                };

                const auto matchSame = [&] (auto & a, auto & b) {
                    overlapsExist = true;
                    return a + room.mRect.midpoint().normalised();
                };

                room.mMovement += std::accumulate(overlapping.begin(), overlapping.end(), HGE::Vector2f(), matchSeparate);
                room.mMovement += std::accumulate(overlappingSame.begin(), overlappingSame.end(), HGE::Vector2f(), matchSame);
            });

            std::for_each(mRooms.begin(), mRooms.end(), [] (auto & room) {
                room.mMovement.x = HGE::roundValueToMultipleOf(room.mMovement.x, 1.0f);
                room.mMovement.y = HGE::roundValueToMultipleOf(room.mMovement.y, 1.0f);
                room.mRect.mPosition += room.mMovement;
                room.mMovement = HGE::Vector2f();
            });
        }
    }

public:
    explicit DungeonGenerator(HGE::DebugComponent* debug) : mDebug(debug) { }
    ~DungeonGenerator() = default;

    const std::vector<Room>& getRooms() {
        return mRooms;
    }

    void generate() {
        mRooms = generateRandomRooms(sNumberOfInitialRooms);
        separateRooms();

        auto removeIt = std::remove_if(mRooms.begin(), mRooms.end(),
                     [] (const auto & room) { return room.mRect.area() < sMinimumRoomArea; });
        mRooms.erase(removeIt, mRooms.end());

        /* vector of midpoints */
        auto midpoints = std::vector<std::pair<int, HGE::Vector2f>>();

        std::transform(mRooms.begin(), mRooms.end(),
                std::back_inserter(midpoints),
                [&] (const auto & room) { return std::make_pair(room.mId, room.mRect.midpoint()); });

        auto triangulation = delaunayTriangulationFromPoints(midpoints);
        auto minimumSpanTree = minimumSpanningTreeFromDelaunayTriangulation(triangulation);
//
//        for(auto const & edge : triangulation.mEdges) {
//            mDebug->drawLine(HGE::Vector2f ( 400 + edge->a()->x(), 300 + edge->a()->y() ),
//                             HGE::Vector2f ( 400 + edge->b()->x(), 300 + edge->b()->y() ),
//                             10.0f,
//                             {255, 255, 255} );
//        }

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


        /* generate paths between rooms */

        /* fill out paths with smaller rooms */

        /* fin and return */
    }

    std::vector<Room> generateRandomRooms(const int numberOfRooms) {
        std::vector<Room> rooms{ };

        for(int i = 0; i < numberOfRooms; ++i) {
            rooms.emplace_back(Room(i));
            rooms.back().mRect = HGE::Rectf(randomPointInCircle(),
                                             { HGE::roundValueToMultipleOf(HGE::randomNumberBetween<float>(sMinimumRoomSize,
                            sMaximumRoomSize), 1.0f),
                      HGE::roundValueToMultipleOf(HGE::randomNumberBetween<float>(sMinimumRoomSize,
                              sMaximumRoomSize), 1.0f) });
        }

        return std::move(rooms);
    }
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
