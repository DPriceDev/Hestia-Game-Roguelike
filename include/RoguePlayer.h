#ifndef HESTIA_ROGUELIKE_ROGUE_PLAYER_H
#define HESTIA_ROGUELIKE_ROGUE_PLAYER_H

#include <framework/ecs/Object.h>
#include <framework/Engine.h>
#include <framework/systems/TickSystem.h>
#include <framework/systems/SpriteSystem.h>
#include <framework/systems/ControlSystem.h>
#include <framework/systems/WorldPositionSystem.h>

#include <util/Logger.h>

class RoguePlayer : public HGE::Object {

    protected:
    HGE::TickComponent* mTickComponent;
    HGE::SpriteComponent* mSpriteComponent;
    HGE::ControlComponent* mControlComponent;
    HGE::WorldPositionComponent* mPositionComponent;

    float mMovementSpeed = 220;

    public:
    RoguePlayer() = default;
    ~RoguePlayer() = default;

    void onCreate() override {
        mTickComponent = HGE::Engine::componentManager()->createComponent<HGE::TickComponent>(getId(), [&] (double deltaTime) { this->tickFunction(deltaTime); });
        mPositionComponent = HGE::Engine::componentManager()->createComponent<HGE::WorldPositionComponent>(getId());
        mSpriteComponent = HGE::Engine::componentManager()->createComponent<HGE::SpriteComponent>(getId());
        mControlComponent = HGE::Engine::componentManager()->createComponent<HGE::ControlComponent>(getId());

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
        mSpriteComponent->mTransform.mRotation += 360/3 * deltaTime;

        auto screenSize = HGE::Engine::instance()->graphicsModule()->getScreenSize();

        if(mControlComponent->getKeyValue(HGE::DOWN_ARROW_KEY)) {
            mPositionComponent->mTransform.mLocalPosition.y -= mMovementSpeed * deltaTime;

            if(mPositionComponent->mTransform.mLocalPosition.y < 0 + mSpriteComponent->mTransform.mScale.y/2) {
                mPositionComponent->mTransform.mLocalPosition.y = 0 + mSpriteComponent->mTransform.mScale.y/2;
            }
        }

        if(mControlComponent->getKeyValue(HGE::UP_ARROW_KEY)) {
            mPositionComponent->mTransform.mLocalPosition.y += mMovementSpeed * deltaTime;

            if(mPositionComponent->mTransform.mLocalPosition.y > screenSize.height() - mSpriteComponent->mTransform.mScale.y/2) {
                mPositionComponent->mTransform.mLocalPosition.y = screenSize.height() - mSpriteComponent->mTransform.mScale.y/2;
            }
        }

        if(mControlComponent->getKeyValue(HGE::LEFT_ARROW_KEY)) {
            mPositionComponent->mTransform.mLocalPosition.x -= mMovementSpeed * deltaTime;

            if(mPositionComponent->mTransform.mLocalPosition.x < 0 + mSpriteComponent->mTransform.mScale.x/2) {
                mPositionComponent->mTransform.mLocalPosition.x = 0 + mSpriteComponent->mTransform.mScale.x/2;
            }
        }

        if(mControlComponent->getKeyValue(HGE::RIGHT_ARROW_KEY)) {
            mPositionComponent->mTransform.mLocalPosition.x += mMovementSpeed * deltaTime;

            if(mPositionComponent->mTransform.mLocalPosition.x > screenSize.width() - mSpriteComponent->mTransform.mScale.x/2) {
                mPositionComponent->mTransform.mLocalPosition.x = screenSize.width() - mSpriteComponent->mTransform.mScale.x/2;
            }
        }
    }
};

#endif