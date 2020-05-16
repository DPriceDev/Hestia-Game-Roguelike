//
// Created by David Price on 14/05/2020.
//

#ifndef HESTIA_ROGUELIKE_MAP_GRID_H
#define HESTIA_ROGUELIKE_MAP_GRID_H

#include <framework/ecs/game_object.h>
#include <framework/systems/tick_system.h>
#include <engine.h>
#include <array>

#include "RoguePlayer.h"
#include "Wall.h"
#include "floor_tile.h"
#include "dungeon_generator.h"

#include <util/logger.h>
#include <framework/systems/debug_system.h>

/**
 *
 */
class MapGrid : public HGE::GameObject {

    HGE::DebugComponent* mDebugComponent;

    static const int MaxGridSize = 10;
    constexpr static float gridStepSize = 2.0f;

    std::array<std::array<HGE::GameObject*, MaxGridSize>, MaxGridSize> mGrid { };

    void onCreate() override {
        mDebugComponent = createComponent<HGE::DebugComponent>(getId());

        auto dungeonGen = DungeonGenerator(mDebugComponent);

        dungeonGen.generate();

        for(auto const & room : dungeonGen.getRooms()) {
            auto roomTile = createObject<FloorTile>();
            roomTile->mPosition->mTransform.mLocalPosition = HGE::Vector2f(400, 300) + (room.mRect.mPosition);
            roomTile->mSprite->mTransform.mScale.x = room.mRect.mSize.x;
            roomTile->mSprite->mTransform.mScale.y = room.mRect.mSize.y;
        }
    }

public:

    void addPlayerToStartRoom(RoguePlayer* roguePlayer) {

    }
};

#endif //HESTIA_ROGUELIKE_MAP_GRID_H
