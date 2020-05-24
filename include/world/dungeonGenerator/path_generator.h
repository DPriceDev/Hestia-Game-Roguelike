//
// Created by David Price on 22/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
#define HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H

#include <queue>
#include <memory>
#include <array>
#include <algorithm>

#include <maths/maths.h>
#include <util/logger.h>

#include "dungeon_grid.h"
#include "room.h"

// todo: pointerfy?
struct Tile {
    int x, y;
    int score{ 9999 };
    bool isVisited{ false };
    bool isInQueue{ false };

    Tile(int x, int y) : x(x), y(y) { }
};

/* todo: pointerfy */
/* todo: is this struct needed? */
struct AdjacentTiles {
    std::array<Tile*, 4> mTiles;

    AdjacentTiles() = default;
    AdjacentTiles(Tile* top, Tile* bottom, Tile* left, Tile* right)
        : mTiles({top, right, bottom, left}) { }

    Tile* & top() { return mTiles[0]; }
    Tile* & right() { return mTiles[1]; }
    Tile* & bottom() { return mTiles[2]; }
    Tile* & left() { return mTiles[3]; }
};

/**
 * todo: make bounds a struct
 * @param bounds
 * @param x
 * @param y
 * @return
 */
static bool isPointInBounds(const std::array<int, 4> &bounds, const int x, const int y) {
    return x <= bounds.at(0) && x >= bounds.at(2)
        && y <= bounds.at(1) && y >= bounds.at(3);
}

/**
 *
 * @param grid
 * @param pTiles
 * @param tile
 * @return
 */
static AdjacentTiles getAdjacentTiles(DungeonGrid &grid, std::vector<std::unique_ptr<Tile>> &pTiles, const Tile* tile) {

    constexpr std::array<int, 4> horizontal = {0, 1, 0, -1};
    constexpr std::array<int, 4> vertical = {1, 0, -1, 0};
    auto adj = AdjacentTiles();
    int index = 0;
    for(auto & adjTile : adj.mTiles) {

        const auto isSameTile = [&tile, &horizontal, &vertical, &index] (const auto & pTile) {
            return pTile->x == tile->x + horizontal[index]
                && pTile->y == tile->y + vertical[index];
        };

        if(!isPointInBounds(grid.getBounds(), tile->x + horizontal[index], tile->y + vertical[index])) {
            continue;
        }

        auto it = std::find_if(pTiles.begin(), pTiles.end(), isSameTile);

        if(it == pTiles.end()) {
            auto ptr = pTiles.emplace_back(
                    std::make_unique<Tile>(tile->x + horizontal[index],tile->y + vertical[index])).get();
            adj.mTiles[index] = ptr;
        } else {
           adj.mTiles[index] = it->get();
        }
        ++index;
    }

    /* move return? */
    return adj;
}

/**
 *
 * @param grid
 * @param roomA
 * @param roomB
 */
static Path generatePath(DungeonGrid &grid, Room* roomA, Room* roomB) {

    // get start
    //roomA->mRect.midpoint();

    std::vector<std::unique_ptr<Tile>> pTiles;
    std::queue<Tile*> tileQueue;

    /* add room corners as 9999 score tiles */

    /* add start square (room midpoint, floor float) */
    auto ptr = pTiles.emplace_back(
            std::make_unique<Tile>(roomA->mRect.mPosition.x,
                                   roomA->mRect.mPosition.y)).get();
    ptr->score = 0;
    tileQueue.push(ptr);

    bool isFinished{ false };
    while(!tileQueue.empty() && !isFinished) {

        auto adjacentTiles = getAdjacentTiles(grid, pTiles, tileQueue.front());
        for(auto & adjTile : adjacentTiles.mTiles) {

            /* todo: do not parse if set as ignore */
            if(adjTile == nullptr) {
                continue;
            }

            if(adjTile->score > tileQueue.front()->score) {
                adjTile->score = tileQueue.front()->score + 1;
            }

            /* todo: if wall of another room or a path, do not add to queue */

            if(!adjTile->isVisited && !adjTile->isInQueue) {
                adjTile->isInQueue = true;
                tileQueue.push(adjTile);
            }

            // todo: get rid of static casts!
            if(adjTile->x == static_cast<int>(roomB->mRect.mPosition.x)
                && adjTile->y == static_cast<int>(roomB->mRect.mPosition.y)) {
                isFinished = true;
            }
        }

        /* remove tile */
        tileQueue.front()->isVisited = true;
        tileQueue.pop();
    }

    const auto getFinishTile = [&roomB] (const std::unique_ptr<Tile> & tile) {
        return tile->x == roomB->mRect.mPosition.x
            && tile->y == roomB->mRect.mPosition.y;
    };

    auto currentTile = std::find_if(pTiles.begin(), pTiles.end(), getFinishTile)->get();
    std::vector<Tile*> pathTiles { currentTile };

    while(currentTile != pTiles.front().get()) {
        auto adjacent = getAdjacentTiles(grid, pTiles, currentTile);

        const auto lowestScore = [] (const auto a, const auto b) {
            return a->score < b->score && b->score >= 0;
        };

        currentTile = *std::min_element(adjacent.mTiles.begin(), adjacent.mTiles.end(), lowestScore);
        pathTiles.push_back(currentTile);
    }


    // when position found, trace back the path by getting the next lowest number.
    // prefer to go in a straight line compared to zig zag if the next lowest is straight, not angled

    // todo: map all paths and get one with lowest number of turns?

    // translate the path into the path class, list of points, list of connected ids

    const auto pathTilesToVector2f = [] (const auto & tile) {
            return HGE::Vector2f(tile->x, tile->y);
    };

    auto path = Path();
    std::transform(pathTiles.begin(), pathTiles.end(), std::back_inserter(path.mNodes), pathTilesToVector2f);
    path.mConnectedRooms.push_back(roomA->mId);
    path.mConnectedRooms.push_back(roomB->mId);
    return path;
}

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
