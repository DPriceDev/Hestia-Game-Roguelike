//
// Created by David Price on 26/05/2020.
//

#include "world/floor_tile.h"

#include <engine.h>
#include <maths/maths.h>

FloorTile::~FloorTile() {
    destroyComponent(mSprite);
    destroyComponent(mPosition);
}

void FloorTile::onCreate() {
    mSprite = createComponent<HGE::SpriteComponent>(getId());
    mPosition = createComponent<HGE::PositionComponent>(getId());

    mSprite->mTransform.mScale.x = 32;
    mSprite->mTransform.mScale.y = 32;

    mSprite->mShader = mContext->mGraphicsModule->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                "./assets/shaders/fragmentShader.fs");

    mSprite->mMaterial = mContext->mGraphicsModule->getMaterial(
            tileFiles[HGE::randomNumberBetween<int>(0, 9)].c_str());
}