#ifndef HESTIA_ROGUELIKE_ROGUE_PLAYER_H
#define HESTIA_ROGUELIKE_ROGUE_PLAYER_H

#include <framework/ecs/object.h>
#include <framework/ecs/GameObject.h>
#include <framework/Engine.h>
#include <framework/systems/TickSystem.h>
#include <framework/systems/SpriteSystem.h>
#include <framework/systems/ControlSystem.h>
#include <framework/systems/WorldPositionSystem.h>
#include <framework/systems/CameraSystem.h>

#include <util/Logger.h>

class RoguePlayer : public HGE::GameObject {

    protected:
    HGE::TickComponent* mTickComponent;
    HGE::SpriteComponent* mSpriteComponent;
    HGE::ControlComponent* mControlComponent;
    HGE::WorldPositionComponent* mPositionComponent;
    HGE::CameraComponent* mCameraComponent;

    float mMovementSpeed = 220;

    public:
    RoguePlayer() = default;
    ~RoguePlayer() override {
        destroyComponent(mTickComponent);
        destroyComponent(mSpriteComponent);
        destroyComponent(mControlComponent);
        destroyComponent(mPositionComponent);
        destroyComponent(mCameraComponent);
    }

    void onCreate() override {
        mTickComponent = createComponent<HGE::TickComponent>(getId());
        mPositionComponent = createComponent<HGE::WorldPositionComponent>(getId());
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

    void tickFunction(double deltaTime) {
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
};

#endif