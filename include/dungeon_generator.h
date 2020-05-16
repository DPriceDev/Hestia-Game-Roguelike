//
// Created by David Price on 15/05/2020.
//

#ifndef HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
#define HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H

#include <cmath>
#include <vector>
#include <random>

#include <maths/MathsTypes.h>
#include <util/Logger.h>

/**
 * Dungeon Room Struct
 */
struct Room {
    int mId;
    HGE::Rectf mRect{ };
    HGE::Vector2f mMovement{ };

    explicit Room(int id) : mId(id) { }
    ~Room() = default;

    bool operator!=(const Room & other) const {
        return this->mId != other.mId;
    }
};

/**
 * Dungeon Generator
 */
class DungeonGenerator {
    constexpr static float S_RADIUS = 100.0f;
    constexpr static float S_RADIUS_SQRD = S_RADIUS * S_RADIUS;
    constexpr static float S_TWO_PI = 2 * M_PI;
    constexpr static float sMinimumRoomSize = 4.0f;
    constexpr static float sMaximumRoomSize = 20.0f;

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


    void separateRooms() {

        bool overlapsExist = true;
        while(overlapsExist) {
            overlapsExist = false;

            for (auto &room : mRooms) {
                for (auto const &other : mRooms) {
                    if (room.mRect.isOverlapping(other.mRect) && room != other) {

                        if(room.mRect.midpoint() != other.mRect.midpoint()) {
                            room.mMovement += (room.mRect.midpoint() - other.mRect.midpoint());
                        } else {
                            room.mMovement += (room.mRect.midpoint().normalised() * 1.0f);
                        }
                        overlapsExist = true;
                    }
                }

//                LOG_DEBUG("seperate rooms", "id: ", room.mId, "vector- x: ", movementVector.x, " y: ", movementVector
//                .y, " --- room position x: ", room.mRect.mPosition.x, " y: ", room.mRect.mPosition.y)
            }

            for(auto & room : mRooms) {
                room.mMovement.x = HGE::roundValueToMultipleOf(room.mMovement.x, 1.0f);
                room.mMovement.y = HGE::roundValueToMultipleOf(room.mMovement.y, 1.0f);
                room.mRect.mPosition += room.mMovement;
                room.mMovement = HGE::Vector2f();
            }
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
    DungeonGenerator() = default;
    ~DungeonGenerator() = default;

    const std::vector<Room>& getRooms() {
        return mRooms;
    }

    void generate() {

        for(int i = 0; i < 100; ++i) {
            mRooms.emplace_back(Room(i));

            mRooms.back().mRect = HGE::Rectf(randomPointInCircle(),
                    { HGE::roundValueToMultipleOf(
                            HGE::randomNumberBetween<float>(sMinimumRoomSize,
                                    sMaximumRoomSize),
                                    1.0f),
                      HGE::roundValueToMultipleOf(
                              HGE::randomNumberBetween<float>(sMinimumRoomSize,
                                      sMaximumRoomSize),
                                      1.0f)});

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

        separateRooms();

        /* move the rooms so they do not overlap */

        /* select the big rooms and discard rest. */

        /* do a delaneay run on rooms and generate a graph? */

        /* do a minimum angle check on graph? */

        /* generate paths between rooms */

        /* fill out paths with smaller rooms */

        /* fin and return */
    }
};

#endif //HESTIA_ROGUELIKE_DUNGEON_GENERATOR_H
