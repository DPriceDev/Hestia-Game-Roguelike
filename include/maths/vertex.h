//
// Created by David Price on 18/05/2020.
//

#ifndef HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H
#define HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H

#include <maths/maths_types.h>

struct Vertex {
    HGE::Vector2f mPosition;
    HGE::Pointf mMagnitude;

    Vertex(HGE::Pointf x, HGE::Pointf y) : mPosition(x, y), mMagnitude(magnitude()) { }
    explicit Vertex(HGE::Vector2f location) : mPosition(location), mMagnitude(magnitude()) { }
    ~Vertex() = default;

    [[nodiscard]] const HGE::Pointf& x() const { return mPosition.x; }
    [[nodiscard]] const HGE::Pointf& y() const { return mPosition.y; }

private:
    [[nodiscard]]
    HGE::Pointf magnitude() const {
        return sqrt((mPosition.x * mPosition.x) + (mPosition.y * mPosition.y));
    }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_MATH_VERTEX_H
