/**
 * Copyright D. price 2020.
 */

#include <engine.h>
#include <graphics/opengl_module.h>

#include "rogue_game_environment.h"

int main() {
    auto engine = std::make_unique<HGE::Engine>();

    engine->useGraphicsModule<HGE::OpenglModule>();

    engine->getGraphicsModule()->setGameTitle("Hestia Roguelike v1.0");

    // TODO: Load inital config and get initial level?
    engine->loadGameEnvironment<RogueGameEnvironment>();
    return 0;
}
