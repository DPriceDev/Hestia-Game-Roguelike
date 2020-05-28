//
// Created by David Price on 26/05/2020.
//

#include "maths/components/triangle.h"

#include <cmath>
#include <algorithm>

/**
 *
 * @return
 */
HGE::Vector2f Triangle::circumcenter() const {

    auto A = b()->x() - a()->x();
    auto B = b()->y() - a()->y();
    auto C = c()->x() - a()->x();
    auto D = c()->y() - a()->y();
    auto E = A * (a()->x() + b()->x()) + B * (a()->y() + b()->y());
    auto F = C * (a()->x() + c()->x()) + D * (a()->y() + c()->y());
    auto G = 2 * (A * (c()->y() - b()->y()) - B * (c()->x() - b()->x()));

    if (abs(G) < 0.000001) {
        auto minx = std::min({ a()->x(), b()->x(), c()->x() });
        auto miny = std::min({ a()->y(), b()->y(), c()->y() });
        auto dx = (std::max({ a()->x(), b()->x(), c()->x() }) - minx) * 0.5;
        auto dy = (std::max({ a()->y(), b()->y(), c()->y() }) - miny) * 0.5;

        return HGE::Vector2f(minx + dx, miny + dy);
    } else {
        return HGE::Vector2f((D * E - B * F) / G, (A * F - C * E) / G);
    }
}

/**
 *
 * @param circumcenter
 * @return
 */
HGE::Pointf Triangle::circumcenterRadius(HGE::Vector2f &circumcenter) const {
    auto dx = circumcenter.x - a()->x();
    auto dy = circumcenter.y - a()->y();
    return sqrt(dx * dx + dy * dy);
}