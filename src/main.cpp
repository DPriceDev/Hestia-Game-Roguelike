/**
 * Copyright D. price 2020.
 */

#include <iostream>
#include <memory>

#include "framework/Engine.h"
#include "framework/GameEnvironment.h"

#include "graphics/OpenGlModule.h"

#include "util/Logger.h"

int main(void) {
    HGE::Engine* engine = HGE::Engine::instance();
    engine->useGraphicsModule<HGE::OpenGlModule>();

    std::unique_ptr<HGE::GameEnvironment> gameEnvironment = std::make_unique<HGE::GameEnvironment>();
    gameEnvironment->Init();
    // TODO: Load inital config and get initial level?
    // TODO: Load inital level and init the gameEnvironment
    
    /* Main Loop whilst window is open. */
    while(engine->instance()->graphicsModule()->isWindowOpen()) {
        gameEnvironment->GameLoop();
    }

    return 0;
}