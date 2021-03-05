#ifndef HESTIA_ROGUELIKE_ROGUE_PLAYER_H
#define HESTIA_ROGUELIKE_ROGUE_PLAYER_H

#include <game/game_object.h>
#include <game/systems/tick_system.h>
#include <game/systems/sprite_system.h>
#include <game/systems/control_system.h>
#include <game/systems/position_system.h>
#include <game/systems/camera_system.h>

class RoguePlayer : public HGE::GameObject {

    HGE::TickComponent *mTickComponent;
    HGE::SpriteComponent *mSpriteComponent;
    HGE::ControlComponent *mControlComponent;
    HGE::PositionComponent *mPositionComponent;
    HGE::CameraComponent *mCameraComponent;

    float mMovementSpeed{ 220 };

    void tickFunction(double deltaTime);

public:
    ~RoguePlayer() override;

    void onCreate() override;
};

#endif