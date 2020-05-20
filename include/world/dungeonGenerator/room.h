//
// Created by David Price on 20/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H
#define HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H

#include <maths/maths_types.h>

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


#endif //HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H
