//
// Created by David Price on 20/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H
#define HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H

#include <maths/maths_types.h>

struct Room {
    int mId;
    HGE::Recti mRect;
    HGE::Vector2i mMovement{ };

    Room(int id, HGE::Recti rect) : mId(id), mRect(rect) { }

    bool operator!=(const Room &other) const {
        return this->mId != other.mId;
    }

    const HGE::Vector2i topRight() const { return mRect.topRight(); }
    const HGE::Vector2i topLeft() const { return mRect.topLeft(); }
    const HGE::Vector2i bottomRight() const { return mRect.bottomRight(); }
    const HGE::Vector2i bottomLeft() const { return mRect.bottomLeft(); }
};


#endif //HESTIA_ROGUELIKE_INCLUDE_WORLD_DUNGEONGENERATOR_ROOM_H
