//
// Created by David Price on 11/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WALL_H
#define HESTIA_ROGUELIKE_WALL_H

#include <framework/ecs/GameObject.h>
#include <framework/systems/SpriteSystem.h>
#include <framework/Engine.h>

#include <maths/HGEMath.h>

class Wall : public HGE::GameObject {

    const std::string backWallTiles[5] = {"./assets/textures/wall_1.png",
                                       "./assets/textures/wall_2.png",
                                       "./assets/textures/wall_crack.png",
                                       "./assets/textures/wall_side_right.png",
                                       "./assets/textures/wall_side_left.png"};
public:
    HGE::SpriteComponent* mSpriteComponent;
    HGE::WorldPositionComponent* mPositionComponent;

    Wall() = default;
    ~Wall() override {
        destroyComponent(mSpriteComponent);
        destroyComponent(mPositionComponent);
    }

    void onCreate() override {
        mSpriteComponent = createComponent<HGE::SpriteComponent>(getId());
        mPositionComponent = createComponent<HGE::WorldPositionComponent>(getId());
        mPositionComponent->mTransform.mLocalPosition.x = 500;
        mPositionComponent->mTransform.mLocalPosition.y = 600;
        mSpriteComponent->mTransform.mScale.x = 32;
        mSpriteComponent->mTransform.mScale.y = mSpriteComponent->mTransform.mScale.x;

        mSpriteComponent->mShader = HGE::Engine::graphicsModule()->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                             "./assets/shaders/fragmentShader.fs");

        mSpriteComponent->mMaterial = HGE::Engine::graphicsModule()->getMaterial
                (backWallTiles[HGE::randomNumberBetween<int>(0, 3)].c_str());
    }

};

#endif //HESTIA_ROGUELIKE_WALL_H
