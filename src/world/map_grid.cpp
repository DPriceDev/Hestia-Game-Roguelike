//
// Created by David Price on 26/05/2020.
//

#include "world/map_grid.h"

#include "world/floor_tile.h"
#include "world/dungeonGenerator/dungeon_generator.h"

void MapGrid::onCreate() {
mDebugComponent = createComponent<HGE::DebugComponent>(getId());

    auto dungeonGen = DungeonGenerator(mDebugComponent);

    dungeonGen.generate();

    for(auto const & room : dungeonGen.getRooms()) {
        auto roomTile = createObject<FloorTile>();
        roomTile->mPosition->mTransform.mLocalPosition.x = (400 + room->mRect.mPosition.x) * 1.0f;
        roomTile->mPosition->mTransform.mLocalPosition.y = (300 + room->mRect.mPosition.y) * 1.0f;

        roomTile->mSprite->mTransform.mScale.x = room->mRect.mSize.x * 1.0f;
        roomTile->mSprite->mTransform.mScale.y = room->mRect.mSize.y * 1.0f;
        roomTile->mSprite->mAlpha = 0.5f;
    }
}

void MapGrid::addPlayerToStartRoom(RoguePlayer *roguePlayer) {

}
