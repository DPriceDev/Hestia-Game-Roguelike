//
// Created by David Price on 26/05/2020.
//

#include "world/wall.h"

#include <engine.h>
#include "framework/ecs/interactors/component_interactor.h"
#include <maths/maths.h>

Wall::~Wall() {
    HGE::ECS::destroyComponent(mContext, mSpriteComponent);
    HGE::ECS::destroyComponent(mContext, mPositionComponent);
}

void Wall::onCreate() {
    mSpriteComponent = HGE::ECS::createComponent<HGE::SpriteComponent>(mContext, getId());
    mPositionComponent = HGE::ECS::createComponent<HGE::PositionComponent>(mContext, getId());
    mPositionComponent->mTransform.mLocalPosition.x = 500;
    mPositionComponent->mTransform.mLocalPosition.y = 600;
    mSpriteComponent->mTransform.mScale.x = 32;
    mSpriteComponent->mTransform.mScale.y = mSpriteComponent->mTransform.mScale.x;

    mSpriteComponent->mShader = mContext->mGraphicsModule->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                         "./assets/shaders/fragmentShader.fs");

    mSpriteComponent->mMaterial = mContext->mGraphicsModule->getMaterial
                                                                       (backWallTiles[HGE::randomNumberBetween<int>(0,
                                                                                                                    3)].c_str());
}
