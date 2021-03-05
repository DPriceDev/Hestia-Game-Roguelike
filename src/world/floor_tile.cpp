//
// Created by David Price on 26/05/2020.
//

#include "world/floor_tile.h"

#include <engine.h>
#include <maths/maths.h>
#include <framework/ecs/interactors/component_interactor.h>

FloorTile::~FloorTile() {
    HGE::ECS::destroyComponent(mContext, mSprite);
    HGE::ECS::destroyComponent(mContext, mPosition);
}

void FloorTile::onCreate() {
    mSprite = HGE::ECS::createComponent<HGE::SpriteComponent>(mContext, getId());
    mPosition = HGE::ECS::createComponent<HGE::PositionComponent>(mContext, getId());

    mSprite->mTransform.mScale.x = 32;
    mSprite->mTransform.mScale.y = 32;

    mSprite->mShader = mContext->mGraphicsModule->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                "./assets/shaders/fragmentShader.fs");

    mSprite->mMaterial = mContext->mGraphicsModule->getMaterial(
            tileFiles[HGE::randomNumberBetween<int>(0, 9)].c_str());
}