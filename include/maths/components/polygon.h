//
// Created by David Price on 17/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_POLYGON_H
#define HESTIA_ROGUELIKE_MATHS_POLYGON_H

#include <vector>

#include "maths/components/edge.h"

struct Polygon {
    std::vector<Edge *> edges{ };
};

#endif //HESTIA_ROGUELIKE_MATHS_POLYGON_H
