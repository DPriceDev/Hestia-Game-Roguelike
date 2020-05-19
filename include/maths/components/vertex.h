//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H
#define HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H

#include <maths/maths_types.h>

struct Vertex {
    int mId;
    HGE::Vector2f mPosition;
    HGE::Pointf mMagnitude;

    Vertex(int id, HGE::Pointf x, HGE::Pointf y) : mId(id), mPosition(x, y), mMagnitude(magnitude()) { }
    explicit Vertex(int id, HGE::Vector2f location) : mId(id), mPosition(location), mMagnitude(magnitude()) { }
    ~Vertex() = default;

    [[nodiscard]] const HGE::Pointf& x() const noexcept { return mPosition.x; }
    [[nodiscard]] const HGE::Pointf& y() const noexcept { return mPosition.y; }

private:
    [[nodiscard]]
    HGE::Pointf magnitude() const {
        return sqrt((mPosition.x * mPosition.x) + (mPosition.y * mPosition.y));
    }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H
