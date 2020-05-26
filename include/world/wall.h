//
// Created by David Price on 11/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WALL_H
#define HESTIA_ROGUELIKE_WALL_H

#include <framework/ecs/game_object.h>
#include <framework/systems/sprite_system.h>

class Wall : public HGE::GameObject {

    const std::string backWallTiles[5] = {"./assets/textures/wall_1.png",
                                       "./assets/textures/wall_2.png",
                                       "./assets/textures/wall_crack.png",
                                       "./assets/textures/wall_side_right.png",
                                       "./assets/textures/wall_side_left.png"};
public:
    HGE::SpriteComponent* mSpriteComponent;
    HGE::PositionComponent* mPositionComponent;

    Wall() = default;
    ~Wall() override;

    void onCreate() override;
};

#endif //HESTIA_ROGUELIKE_WALL_H
