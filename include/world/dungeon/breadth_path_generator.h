//
// Created by David Price on 22/05/2020.
//

#ifndef HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H
#define HESTIA_ROGUELIKE_WORLD_DUNGEONGENERATOR_PATH_GENERATOR_H

#include <memory>
#include <array>
#include <map>
#include <algorithm>
#include <exception>

#include <maths/maths.h>
#include <maths/maths_types.h>
#include <util/logger.h>

#include "grid_tile.h"

class BreadthPathingException : public std::exception {

};

template<class Type, class ScoreOperator, class PathingOperator>
class BreadthPathGenerator {
    const std::array<HGE::Vector2i, 4> sOffsets{
            HGE::Vector2i{ 0, 1 },
            HGE::Vector2i{ 1, 0 },
            HGE::Vector2i{ 0, -1 },
            HGE::Vector2i{ -1, 0 }};

    HGE::Grid<Type> &mGrid;
    ScoreOperator mScoreOperator;
    PathingOperator mPathingOperator;

    static constexpr int sMaxScore = 99999999;

    /**
     * Internal Tile Class
     */
    struct Tile {
        HGE::Vector2i mPosition{ 0, 0 };
        int mScore{ sMaxScore };
        bool mVisited{ false };

        Tile() = default;

        Tile(const HGE::Vector2i &position, const int &score, bool visited)
                : mPosition(position), mScore(score), mVisited(visited) { }
    };

    /**
     * Internal Pathing Operator Struct
     */
     struct PathingData {
         HGE::Vector2i mStart;
         HGE::Vector2i mFinish;
         std::array<Tile, 4> mAdjacentTiles{ };
         HGE::Vector2i mCurrentTile;
         HGE::Vector2i mPreviousTile;
         int mCurrentScore;

         PathingData(const HGE::Vector2i &start, const HGE::Vector2i &finish, const int &currentScore)
             : mStart(start),
               mFinish(finish),
               mCurrentTile(start),
               mPreviousTile(start),
               mCurrentScore(currentScore) { }
     };

    auto breadthSearch(const HGE::Vector2i &start, const HGE::Vector2i &finish) {

        auto startTile = Tile(start, 0, true);
        auto tileGrid = HGE::Grid<Tile>(mGrid.gridParameters());
        tileGrid.at(start.x, start.y) = startTile;
        auto tileQueue = std::queue<Tile>{{ startTile }};
        bool endReached{ false };

        while (!tileQueue.empty() && !endReached) {
            for (auto &offset: sOffsets) {
                auto adjPosition = tileQueue.front().mPosition + offset;

                if (adjPosition == finish) {
                    endReached = true;
                    auto newTile = Tile(adjPosition, sMaxScore, true);
                    tileGrid.at(adjPosition.x, adjPosition.y) = newTile;
                    tileQueue.push(newTile);
                    break;
                }

                if (mGrid.isPointInGrid(adjPosition.x, adjPosition.y)) {
                    auto adjTile = tileGrid.at(adjPosition.x, adjPosition.y);
                    auto newScore = mScoreOperator(adjPosition, mGrid.at(adjPosition.x, adjPosition.y), tileQueue.front().mScore);

                    if (!adjTile.mVisited) {
                        auto newTile = Tile(adjPosition, newScore, true);
                        tileGrid.at(adjPosition.x, adjPosition.y) = newTile;
                        if (newScore != sMaxScore) {
                            tileQueue.push(newTile);
                        }
                    } else if (adjTile.mScore > newScore) {
                        adjTile.mScore = newScore;
                    }
                }
            }
            tileQueue.pop();
        }
        return tileGrid;
    }

    auto traceBackPath(HGE::Grid<Tile> &tileGrid, const HGE::Vector2i &start,
                       const HGE::Vector2i &finish, const std::vector<int> &tileIds) -> Path {
        std::vector<HGE::Vector2i> pathNodes{ finish };

        auto pathingData = PathingData(finish, start, tileGrid.at(finish.x, finish.y).mScore);

        while (pathingData.mCurrentTile != start) {

            const auto createAdjacent = [&tileGrid, &pathingData] (const auto &offset) {
                const auto adjPosition = pathingData.mCurrentTile + offset;

                if(tileGrid.isPointInGrid(adjPosition.x, adjPosition.y)) {
                  return tileGrid.at(adjPosition.x, adjPosition.y);
                } else {
                  return Tile(adjPosition, sMaxScore, true);
                }
            };

            std::transform(sOffsets.begin(), sOffsets.end(),
                           pathingData.mAdjacentTiles.begin(), createAdjacent);

            auto newTile = mPathingOperator(pathingData);
            pathingData.mPreviousTile = pathingData.mCurrentTile;
            pathingData.mCurrentTile = newTile.mPosition;
            pathNodes.push_back(newTile.mPosition);

            if (newTile.mScore >= sMaxScore) {
                throw BreadthPathingException();
            }
        }
        return Path(pathNodes, tileIds);
    }

public:
    BreadthPathGenerator(HGE::Grid<Type> &grid,
                         ScoreOperator scoreOperator,
                         PathingOperator pathingOperator)
            : mGrid(grid),
            mScoreOperator(scoreOperator),
            mPathingOperator(pathingOperator) { }

    ~BreadthPathGenerator() = default;

    Path generatePath(const HGE::Vector2i &start,
                      const HGE::Vector2i &finish,
                      const std::vector<int> &tileIds) {
        auto tileGrid = breadthSearch(start, finish);
        return traceBackPath(tileGrid, start, finish, tileIds);
    }
};

#endif //HESTIA_ROGUELIKE_WORLD_DUNGEON_GENERATOR_PATH_GENERATOR_H
