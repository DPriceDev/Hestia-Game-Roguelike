//
// Created by David Price on 14/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MAP_GRID_H
#define HESTIA_ROGUELIKE_MAP_GRID_H

#include <array>

#include <framework/ecs/game_object.h>
#include <framework/systems/debug_system.h>

#include "player/rogue_player.h"

/**
 *
 */
class MapGrid : public HGE::GameObject {

    HGE::DebugComponent *mDebugComponent;

    static const int MaxGridSize = 10;
    constexpr static float gridStepSize = 2.0f;

    std::array<std::array<HGE::GameObject *, MaxGridSize>, MaxGridSize> mGrid{ };

    void onCreate() override;

public:
    void addPlayerToStartRoom(RoguePlayer *roguePlayer);
};

#endif //HESTIA_ROGUELIKE_MAP_GRID_H
