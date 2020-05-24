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
#include <util/logger.h>

#include <grid.h>
#include "grid_tile.h"

class BreadthPathGenerator {
    const std::array<HGE::Vector2i, 4> sOffsets{
        HGE::Vector2i { 0, 1 },
        HGE::Vector2i { 1, 0 },
        HGE::Vector2i { 0, -1 },
        HGE::Vector2i { -1, 0 } };

    struct Tile {
        HGE::Vector2i mPosition;
        int mScore{ 99999999 };
        bool mVisited{ false };

        Tile() = default;
        Tile(const HGE::Vector2i &position, const int &score, bool visited)
                : mPosition(position), mScore(score), mVisited(visited) { }
    };

    /**
     *
     */
    template<class Type>
    auto breadthSearch(const AAF::Grid2D<Type> &grid, const HGE::Vector2i &start, const HGE::Vector2i &finish) {

        auto startTile = Tile(start, 0, true);
        auto tileGrid = AAF::Grid2D<Tile>(grid.gridParameters());
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
    auto traceBackPath(AAF::Grid2D<Tile> &tileGrid, const HGE::Vector2i &start, const HGE::Vector2i &finish) {

        std::vector<HGE::Vector2i> pathNodes{ finish };
        HGE::Vector2i currentNode = finish;

        while(currentNode != start) {

            /* get valid adjacent */
            auto adjacent = std::vector<Tile>();
            for(const auto & offset : sOffsets) {
                auto adjPosition = currentNode + offset;
                if(tileGrid.isPointInGrid(adjPosition.x, adjPosition.y)) {
                    adjacent.push_back(tileGrid.at(adjPosition.x, adjPosition.y));
                }
            }

            /* check for lowest score */
            const auto lowestScore = [] (const auto & a, const auto & b) {
                return a.mScore < b.mScore;
            };
            auto it = std::min_element(adjacent.begin(), adjacent.end(), lowestScore);

            /* set it as current node and add to pathNodes */
            currentNode = it->mPosition;
            pathNodes.push_back(it->mPosition);
        }

        auto path = Path();
        std::transform(pathNodes.begin(), pathNodes.end(), std::back_inserter(path.mNodes), [] (const auto & node) {
            return HGE::Vector2f(node.x, node.y);
        });

        return path;
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
    Path generatePath(const AAF::Grid2D<Type> &grid, const HGE::Vector2i &start, const HGE::Vector2i &finish) {
        auto tileGrid = breadthSearch(grid, start, finish);
        return traceBackPath(tileGrid, start, finish);
    }
};

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
