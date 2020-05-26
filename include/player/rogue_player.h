#ifndef HESTIA_ROGUELIKE_ROGUE_PLAYER_H
#define HESTIA_ROGUELIKE_ROGUE_PLAYER_H

#include <framework/ecs/game_object.h>
#include <framework/systems/tick_system.h>
#include <framework/systems/sprite_system.h>
#include <framework/systems/control_system.h>
#include <framework/systems/position_system.h>
#include <framework/systems/camera_system.h>

class RoguePlayer : public HGE::GameObject {

    HGE::TickComponent* mTickComponent;
    HGE::SpriteComponent* mSpriteComponent;
    HGE::ControlComponent* mControlComponent;
    HGE::PositionComponent* mPositionComponent;
    HGE::CameraComponent* mCameraComponent;

    float mMovementSpeed { 220 };
    void tickFunction(double deltaTime);

    public:
    RoguePlayer() = default;
    ~RoguePlayer() override;
    void onCreate() override;
};

#endif