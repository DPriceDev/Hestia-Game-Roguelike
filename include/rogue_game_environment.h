#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <game/game_envrionment.h>
#include <context.h>

class RogueGameEnvironment : public HGE::GameEnvironment {

public:
    RogueGameEnvironment(HGE::Context* context) : HGE::GameEnvironment(context) { }

    ~RogueGameEnvironment() override = default;

    void beginGame() override;

    void gameLoop(const double &deltaTime) override;

    void endGame() override;
};

#endif