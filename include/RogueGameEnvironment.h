#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <framework/Engine.h>
#include <framework/GameEnvironment.h>

#include "RoguePlayer.h"
#include "Wall.h"

class RogueGameEnvironment : public HGE::GameEnvironment {

    public:
    RogueGameEnvironment() = default;
    ~RogueGameEnvironment() override = default;

    void beginGame() override {
        HGE::Engine::instance()->objectManager()->CreateObject<RoguePlayer>();
        HGE::Engine::instance()->objectManager()->CreateObject<Wall>();
    }

    void gameLoop() override { }

    void endGame() override {

    }
};

#endif