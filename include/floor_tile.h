//
// Created by David Price on 14/05/2020.
//

#ifndef HESTIA_ROGUELIKE_FLOOR_TILE_H
#define HESTIA_ROGUELIKE_FLOOR_TILE_H

#include <string>

#include <framework/ecs/GameObject.h>
#include <framework/systems/SpriteSystem.h>
#include <framework/Engine.h>

#include <maths/HGEMath.h>

class FloorTile : public HGE::GameObject {

     const std::string tileFiles[10] = {"./assets/textures/floor_1.png",
                                       "./assets/textures/floor_2.png",
                                       "./assets/textures/floor_3.png",
                                       "./assets/textures/floor_4.png",
                                       "./assets/textures/floor_5.png",
                                       "./assets/textures/floor_6.png",
                                       "./assets/textures/floor_7.png",
                                       "./assets/textures/floor_8.png",
                                       "./assets/textures/floor_9.png",
                                       "./assets/textures/floor_10.png"};
public:
    HGE::SpriteComponent* mSprite;
    HGE::WorldPositionComponent* mPosition;

    FloorTile() = default;
    ~FloorTile() override {
        destroyComponent(mSprite);
        destroyComponent(mPosition);
    }

    void onCreate() override {
        mSprite = createComponent<HGE::SpriteComponent>(getId());
        mPosition = createComponent<HGE::WorldPositionComponent>(getId());

        mSprite->mTransform.mScale.x = 32;
        mSprite->mTransform.mScale.y = 32;

        mSprite->mShader = HGE::Engine::graphicsModule()->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                    "./assets/shaders/fragmentShader.fs");

        mSprite->mMaterial = HGE::Engine::graphicsModule()->getMaterial(
                tileFiles[HGE::randomNumberBetween<int>(0, 9)].c_str());
    }
};

#endif //HESTIA_ROGUELIKE_INCLUDE_FLOOR_TILE_H
