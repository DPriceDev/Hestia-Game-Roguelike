#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <framework/GameEnvironment.h>

#include "RoguePlayer.h"
#include "Wall.h"
#include "map_grid.h"

class RogueGameEnvironment : public HGE::GameEnvironment {

    public:
    RogueGameEnvironment() = default;
    ~RogueGameEnvironment() override = default;

    void beginGame() override {
        createObject<MapGrid>();
    }

    void gameLoop(const double & deltaTime) override { }

    void endGame() override {

    }
};

#endif