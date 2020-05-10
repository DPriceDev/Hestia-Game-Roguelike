#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <framework/Engine.h>
#include <framework/GameEnvironment.h>

#include "RoguePlayer.h"

class RogueGameEnvironment : public HGE::GameEnvironment {

    public:
    RogueGameEnvironment() = default;
    ~RogueGameEnvironment() = default;

    void beginGame() override {
        HGE::Engine::instance()->objectManager()->CreateObject<RoguePlayer>();
    }

    void gameLoop() override { }

    void endGame() override {

    }
};

#endif