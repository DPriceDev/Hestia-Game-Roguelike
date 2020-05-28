/**
 * Copyright D. price 2020.
 */

#include <engine.h>
#include <graphics/opengl_module.h>

#include "rogue_game_environment.h"

int main() {
    auto *engine = HGE::Engine::instance();
    HGE::Engine::useGraphicsModule<HGE::OpenglModule>();
    HGE::Engine::graphicsModule()->setGameTitle("Hestia Roguelike v1.0");

    // TODO: Load inital config and get initial level?
    engine->loadGameEnvironment<RogueGameEnvironment>();
    return 0;
}
