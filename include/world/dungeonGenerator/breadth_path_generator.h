//
// Created by David Price on 22/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
#define HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H

#include <queue>
#include <memory>
#include <array>
#include <map>
#include <algorithm>

#include <maths/maths.h>
#include <maths/maths_types.h>
#include <util/logger.h>

#include "grid_tile.h"

class BreadthPathGenerator {
    const std::array<HGE::Vector2i, 4> sOffsets{
        HGE::Vector2i { 0, 1 },
        HGE::Vector2i { 1, 0 },
        HGE::Vector2i { 0, -1 },
        HGE::Vector2i { -1, 0 } };

    const static int sMaxScore = 99999999;

    struct Tile {
        HGE::Vector2i mPosition { 0,0 };
        int mScore { sMaxScore };
        bool mVisited { false };

        Tile() = default;
        Tile(const HGE::Vector2i &position, const int &score, bool visited)
                : mPosition(position), mScore(score), mVisited(visited) { }
    };

    /**
     *
     */
    template<class Type>
    auto breadthSearch(const HGE::Grid<Type> &grid, const HGE::Vector2i &start, const HGE::Vector2i &finish) {

        auto startTile = Tile(start, 0, true);
        auto tileGrid = HGE::Grid<Tile>(grid.gridParameters());
        tileGrid.at(start.x, start.y) = startTile;
        auto tileQueue = std::queue<Tile>{ { startTile } };
        bool endReached{ false };

        while(!tileQueue.empty() && !endReached) {

            auto newScore = tileQueue.front().mScore + 1;
            for(auto & offset: sOffsets) {
                auto adjPosition = tileQueue.front().mPosition + offset;
                if(grid.isPointInGrid(adjPosition.x, adjPosition.y)) {

                    auto adjTile = tileGrid.at(adjPosition.x, adjPosition.y);

                    if(!adjTile.mVisited) {
                        auto newTile = Tile(adjPosition, newScore, true);
                        tileGrid.at(adjPosition.x, adjPosition.y) = newTile;
                        tileQueue.push(newTile);
                    } else if (adjTile.mScore > newScore) {
                        adjTile.mScore = newScore;
                    }

                    if(adjPosition == finish) {
                        endReached = true;
                        break;
                    }
                }
            }
            tileQueue.pop();
        }
        return tileGrid;
    }

    /**
     *
     */
    auto traceBackPath(HGE::Grid<Tile> &tileGrid, const HGE::Vector2i &start,
                       const HGE::Vector2i &finish, const std::vector<int> &tileIds) {
        std::vector<HGE::Vector2i> pathNodes{ finish };
        HGE::Vector2i currentNode = finish;

        while(currentNode != start) {
            auto adjacent = std::vector<Tile>();

            const auto createAdjacent = [&tileGrid, &currentNode] (const auto & offset) {
                const auto adjPosition = currentNode + offset;
                return tileGrid.at(adjPosition.x, adjPosition.y);
            };

            const auto isInGrid = [&tileGrid, &currentNode] (const auto & offset) {
                const auto adjPosition = currentNode + offset;
                return tileGrid.isPointInGrid(adjPosition.x, adjPosition.y);
            };

            constexpr auto lowestScore = [] (const auto & a, const auto & b) {
                return a.mScore < b.mScore;
            };

            ATA::transform_if(sOffsets.begin(), sOffsets.end(),
                              std::back_inserter(adjacent), createAdjacent, isInGrid);

            auto it = std::min_element(adjacent.begin(), adjacent.end(), lowestScore);

            currentNode = it->mPosition;
            pathNodes.push_back(it->mPosition);
        }

        return Path(pathNodes, tileIds);
    }

public:
    BreadthPathGenerator() = default;
    ~BreadthPathGenerator() = default;

    /**
     *
     * @tparam Type
     * @param grid
     * @param start
     * @param finish
     * @return
     */
    template<class Type>
    Path generatePath(const HGE::Grid<Type> &grid, const HGE::Vector2i &start,
                      const HGE::Vector2i &finish, const std::vector<int> &tileIds) {
        auto tileGrid = breadthSearch(grid, start, finish);
        return traceBackPath(tileGrid, start, finish, tileIds);
    }
};

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
