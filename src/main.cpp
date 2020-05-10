/**
 * Copyright D. price 2020.
 */

#include <iostream>
#include <memory>

#include <framework/Engine.h>
#include <graphics/OpenGlModule.h>
#include <util/Logger.h>

#include "RogueGameEnvironment.h"

int main(void) {
    HGE::Engine* engine = HGE::Engine::instance();
    engine->useGraphicsModule<HGE::OpenGlModule>();
    engine->graphicsModule()->setGameTitle("Hestia Roguelike v1.0");

    engine->loadGameEnvironment<RogueGameEnvironment>();

    // TODO: Load inital config and get initial level?
    // TODO: Load inital level and init the gameEnvironment
    
    /* Main Loop whilst window is open. */
    while(engine->instance()->graphicsModule()->isWindowOpen()) {
        //gameEnvironment->GameLoop();
    }

    return 0;
}