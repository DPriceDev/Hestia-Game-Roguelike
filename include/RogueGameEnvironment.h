#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <framework/GameEnvironment.h>

#include "RoguePlayer.h"
#include "Wall.h"

class RogueGameEnvironment : public HGE::GameEnvironment {

    public:
    RogueGameEnvironment() = default;
    ~RogueGameEnvironment() override = default;

    void beginGame() override {
        createObject<RoguePlayer>();
        createObject<Wall>();
    }

    void gameLoop(const double & deltaTime) override { }

    void endGame() override {

    }
};

#endif