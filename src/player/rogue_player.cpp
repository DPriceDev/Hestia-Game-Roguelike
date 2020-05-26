//
// Created by David Price on 26/05/2020.
//

#include "player/rogue_player.h"

#include <engine.h>

RoguePlayer::~RoguePlayer() {
    destroyComponent(mTickComponent);
    destroyComponent(mSpriteComponent);
    destroyComponent(mControlComponent);
    destroyComponent(mPositionComponent);
    destroyComponent(mCameraComponent);
}

void RoguePlayer::onCreate() {
    mTickComponent = createComponent<HGE::TickComponent>(getId());
    mPositionComponent = createComponent<HGE::PositionComponent>(getId());
    mSpriteComponent = createComponent<HGE::SpriteComponent>(getId());
    mControlComponent = createComponent<HGE::ControlComponent>(getId());
    mCameraComponent = createComponent<HGE::CameraComponent>(getId(), HGE::CameraView(800, 600), true);

    mTickComponent->mTickFunction = [&] (double deltaTime) { this->tickFunction(deltaTime); };

    mPositionComponent->mTransform.mLocalPosition.x = 400;
    mPositionComponent->mTransform.mLocalPosition.y = 300;
    mSpriteComponent->mTransform.mScale.x = 100;
    mSpriteComponent->mTransform.mScale.y = mSpriteComponent->mTransform.mScale.x;

    mSpriteComponent->mShader = HGE::Engine::graphicsModule()->getShader("./assets/shaders/basicSpriteVertexShader.vs",
                                                                         "./assets/shaders/fragmentShader.fs");

    mSpriteComponent->mMaterial = HGE::Engine::graphicsModule()->getMaterial("./assets/textures/smileyFace.png");

    mControlComponent->addKey(HGE::UP_ARROW_KEY);
    mControlComponent->addKey(HGE::DOWN_ARROW_KEY);
    mControlComponent->addKey(HGE::LEFT_ARROW_KEY);
    mControlComponent->addKey(HGE::RIGHT_ARROW_KEY);
}

void RoguePlayer::tickFunction(double deltaTime) {
    mSpriteComponent->mTransform.mRotation += 180 * deltaTime;

    if(mControlComponent->getKeyValue(HGE::DOWN_ARROW_KEY)) {
        mPositionComponent->mTransform.mLocalPosition.y -= mMovementSpeed * deltaTime;
    }

    if(mControlComponent->getKeyValue(HGE::UP_ARROW_KEY)) {
        mPositionComponent->mTransform.mLocalPosition.y += mMovementSpeed * deltaTime;
    }

    if(mControlComponent->getKeyValue(HGE::LEFT_ARROW_KEY)) {
        mPositionComponent->mTransform.mLocalPosition.x -= mMovementSpeed * deltaTime;
    }

    if(mControlComponent->getKeyValue(HGE::RIGHT_ARROW_KEY)) {
        mPositionComponent->mTransform.mLocalPosition.x += mMovementSpeed * deltaTime;
    }

    mCameraComponent->mCameraView.mViewportPosition = mPositionComponent->mTransform.mLocalPosition;
    mCameraComponent->mCameraView.mViewportPosition.x -= mCameraComponent->mCameraView.mViewportSize.x/2;
    mCameraComponent->mCameraView.mViewportPosition.y -= mCameraComponent->mCameraView.mViewportSize.y/2;
}