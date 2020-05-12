/**
 * Copyright D. price 2020.
 */

#include <framework/Engine.h>
#include <graphics/OpenGlModule.h>

#include "RogueGameEnvironment.h"

int main() {
    auto* engine = HGE::Engine::instance();
    HGE::Engine::useGraphicsModule<HGE::OpenGlModule>();
    HGE::Engine::graphicsModule()->setGameTitle("Hestia Roguelike v1.0");

    // TODO: Load inital config and get initial level?
    engine->loadGameEnvironment<RogueGameEnvironment>();
    return 0;
}