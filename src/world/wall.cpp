//
// Created by David Price on 26/05/2020.
//

#include "world/wall.h"

#include <engine.h>
#include <maths/maths.h>

Wall::~Wall() {
    destroyComponent(mSpriteComponent);
    destroyComponent(mPositionComponent);
}

void Wall::onCreate() {
    mSpriteComponent = createComponent<HGE::SpriteComponent>(getId());
    mPositionComponent = createComponent<HGE::PositionComponent>(getId());
    mPositionComponent->mTransform.mLocalPosition.x = 500;
    mPositionComponent->mTransform.mLocalPosition.y = 600;
    mSpriteComponent->mTransform.mScale.x = 32;
    mSpriteComponent->mTransform.mScale.y = mSpriteComponent->mTransform.mScale.x;

    mSpriteComponent->mShader = HGE::Engine::graphicsModule()->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                         "./assets/shaders/fragmentShader.fs");

    mSpriteComponent->mMaterial = HGE::Engine::graphicsModule()->getMaterial
            (backWallTiles[HGE::randomNumberBetween<int>(0, 3)].c_str());
}
