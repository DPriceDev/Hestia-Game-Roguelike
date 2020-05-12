//
// Created by David Price on 11/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WALL_H
#define HESTIA_ROGUELIKE_WALL_H

#include <framework/ecs/Object.h>
#include <framework/systems/SpriteSystem.h>
#include <framework/Engine.h>

class Wall : public HGE::Object {

    HGE::SpriteComponent* mSpriteComponent;
    HGE::WorldPositionComponent* mPositionComponent;

public:
    Wall() {

    }
    ~Wall() {

    }

    void onCreate() override {
        mSpriteComponent = HGE::Engine::componentManager()->createComponent<HGE::SpriteComponent>(getId());
        mPositionComponent = HGE::Engine::componentManager()->createComponent<HGE::WorldPositionComponent>(getId());
        mPositionComponent->mTransform.mLocalPosition.x = 500;
        mPositionComponent->mTransform.mLocalPosition.y = 600;
        mSpriteComponent->mTransform.mScale.x = 50;
        mSpriteComponent->mTransform.mScale.y = mSpriteComponent->mTransform.mScale.x;

        mSpriteComponent->mShader = HGE::Engine::graphicsModule()->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                             "./assets/shaders/fragmentShader.fs");

        mSpriteComponent->mMaterial = HGE::Engine::graphicsModule()->getMaterial("./assets/textures/smileyFace.png");
    }

};

#endif //HESTIA_ROGUELIKE_WALL_H
