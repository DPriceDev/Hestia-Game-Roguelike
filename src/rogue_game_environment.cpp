//
// Created by David Price on 26/05/2020.
//

#include "rogue_game_environment.h"
#include "framework/ecs/interactors/object_interactor.h"

#include "world/map_grid.h"

void RogueGameEnvironment::beginGame() {
    HGE::ECS::createObject<MapGrid>(mContext);
}

void RogueGameEnvironment::gameLoop(const double &deltaTime) { }

void RogueGameEnvironment::endGame() { }