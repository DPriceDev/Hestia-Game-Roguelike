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

#include "delaunay.h"

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
    /* todo: still sometimes fails, max iteration cap? before reset? or better method? */
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

        LOG_DEBUG("Dungeon Generator",
                  "id: ",
                  mRooms.back().mId,
                  " generated point- size x: ",
                  mRooms.back().mRect.mSize.x,
                  " size y: ",
                  mRooms.back().mRect.mSize.y,
                  " pos x: ",
                  mRooms.back().mRect.mPosition.x,
                  " pos y: ",
                  mRooms.back().mRect.mPosition.y);
    }

public:
    explicit DungeonGenerator(HGE::DebugComponent* debug) : mDebug(debug) { }
    ~DungeonGenerator() = default;

    const std::vector<Room>& getRooms() {
        return mRooms;
    }

    void generate() {

        for(int i = 0; i < sNumberOfInitialRooms; ++i) {
            mRooms.emplace_back(Room(i));

            mRooms.back().mRect = HGE::Rectf(randomPointInCircle(),
                    { HGE::roundValueToMultipleOf(HGE::randomNumberBetween<float>(sMinimumRoomSize,
                            sMaximumRoomSize), 1.0f),
                      HGE::roundValueToMultipleOf(HGE::randomNumberBetween<float>(sMinimumRoomSize,
                              sMaximumRoomSize), 1.0f) });

            LOG_DEBUG("Dungeon Generator",
                    "id: ",
                    mRooms.back().mId,
                    " generated point- pos x: ",
                      mRooms.back().mRect.mPosition.x,
                      " pos y: ",
                      mRooms.back().mRect.mPosition.y,
                      "midpoint: ",
                      mRooms.back().mRect.midpoint().x,
                      mRooms.back().mRect.midpoint().y)
        }

        /* move the rooms so they do not overlap */
        separateRooms();

        /* select the big rooms and discard rest. */
        mRooms.erase(std::remove_if(mRooms.begin(), mRooms.end(), [] (const auto & room) {
            return room.mRect.area() < sMinimumRoomArea;
        }), mRooms.end());

        /* vector of midpoints */
        auto midpoints = std::vector<HGE::Vector2f>();
        std::transform(mRooms.begin(), mRooms.end(), std::back_inserter(midpoints), [&] (const auto & room) {
            return room.mRect.midpoint();
        });




        /* do a delaneay run on rooms and generate a graph? */
        auto delaunay = Triangulation();
        auto testVec = std::vector<HGE::Vector2f>();
        testVec.emplace_back(-30.0f, -30.0f );
        testVec.emplace_back(50.0f, 100.0f );
        testVec.emplace_back(-50.0f, -100.0f );
        testVec.emplace_back(20.0f, 150.0f );
        testVec.emplace_back(-30.0f, 60.0f );
        testVec.emplace_back(-120.0f, -100.0f );
        testVec.emplace_back(90.0f, -150.0f );


        delaunay.triangulate(midpoints);


        for(auto const & triangle : delaunay.mTriangles) {
            mDebug->drawLine(HGE::Vector2f ( 400 + triangle->a->x, 300 + triangle->a->y ),
                             HGE::Vector2f ( 400 + triangle->b->x, 300 + triangle->b->y ), 10.0f, {255, 255, 255} );

            mDebug->drawLine(HGE::Vector2f ( 400 + triangle->b->x, 300 + triangle->b->y ),
                             HGE::Vector2f ( 400 + triangle->c->x, 300 + triangle->c->y ), 10.0f, {255, 255, 255});

            mDebug->drawLine(HGE::Vector2f ( 400 + triangle->c->x, 300 + triangle->c->y ),
                             HGE::Vector2f ( 400 + triangle->a->x, 300 + triangle->a->y ), 10.0f, {255, 255, 255} );

//            mDebug->drawCircle(HGE::Vector2f ( triangle->circumcenter().x + 400.0f , triangle->circumcenter().y + 300.0f ),
//                    triangle->circumcenterRadius(), 10.0f, {255, 0, 0});
        }

//        for(auto const & edge : delaunay.mEdges) {
//            mDebug->drawLine(HGE::Vector2f ( 400 + edge->a->x, 300 + edge->a->y ),
//                             HGE::Vector2f ( 400 + edge->b->x, 300 + edge->b->y ), 10.0f );
//        }

        float index = 0.0f;
        float step = 1.0f / delaunay.mVertices.size();
        for(auto const & vert : delaunay.mVertices) {
            mDebug->drawCircle(HGE::Vector2f ( vert->x + 400.0f , vert->y + 300.0f ),
                    10.0f, 10.0f, {(step * index), 1.0f - (step * index), 0});
            index += 1.0f;
        }

//        LOG_DEBUG("Trangulation!", "circumcenter",
//                delaunay.mTriangles.begin()->circumcenter().x,
//                delaunay.mTriangles.begin()->circumcenter().y)


        /* do a minimum angle check on graph? */

        /* generate paths between rooms */

        /* fill out paths with smaller rooms */

        /* fin and return */
    }
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
