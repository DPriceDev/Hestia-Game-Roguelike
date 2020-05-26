#ifndef HESTIA_ROGUE_GAME_ENVIRONMENT_H
#define HESTIA_ROGUE_GAME_ENVIRONMENT_H

#include <framework/game_envrionment.h>

class RogueGameEnvironment : public HGE::GameEnvironment {

    public:
    RogueGameEnvironment() = default;
    ~RogueGameEnvironment() override = default;

    void beginGame() override;
    void gameLoop(const double & deltaTime) override;
    void endGame() override;
};

#endif