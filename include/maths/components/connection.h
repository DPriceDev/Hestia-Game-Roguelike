//
// Created by David Price on 19/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MATHS_COMPONENTS_CONNECTION_H
#define HESTIA_ROGUELIKE_MATHS_COMPONENTS_CONNECTION_H

struct Connection {
    int mA, mB;
    float mLength;

    Connection(int a, int b, float length) : mA(a), mB(b), mLength(length) { }
};

#endif //HESTIA_ROGUELIKE_MATHS_COMPONENTS_CONNECTION_H
