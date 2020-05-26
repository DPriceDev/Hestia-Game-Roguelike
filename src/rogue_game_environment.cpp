//
// Created by David Price on 26/05/2020.
//

#include "rogue_game_environment.h"

#include "world/map_grid.h"

void RogueGameEnvironment::beginGame() {
    createObject<MapGrid>();
}

void RogueGameEnvironment::gameLoop(const double & deltaTime) { }

void RogueGameEnvironment::endGame() { }