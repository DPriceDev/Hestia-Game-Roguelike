//
// Created by David Price on 26/05/2020.
//

#include "world/map_grid.h"

#include "world/floor_tile.h"
#include "world/dungeon/dungeon_generator.h"

void MapGrid::onCreate() {
    mDebugComponent = createComponent<HGE::DebugComponent>(getId());

    auto dungeonGen = DungeonGenerator(mDebugComponent);

    auto dungeon = dungeonGen.generate();

    for (auto const &room : dungeon.mMainRooms) {
        auto roomTile = createObject<FloorTile>();
        roomTile->mPosition->mTransform.mLocalPosition.x = (400 + room->mRect.position().x) * 1.0f;
        roomTile->mPosition->mTransform.mLocalPosition.y = (300 + room->mRect.position().y) * 1.0f;

        roomTile->mSprite->mTransform.mScale.x = room->mRect.size().x * 1.0f;
        roomTile->mSprite->mTransform.mScale.y = room->mRect.size().y * 1.0f;
        roomTile->mSprite->mAlpha = 0.5f;
    }
}

void MapGrid::addPlayerToStartRoom(RoguePlayer *roguePlayer) {

}
