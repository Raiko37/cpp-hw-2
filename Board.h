#pragma once

#include <vector>
#include <random>
#include <utility>
#include "Color.h"

class Board {
public:
    Board(int width, int height);

    bool isValidPosition(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    Color getGem(int x, int y) const;
    void setGem(int x, int y, Color color);

    void swapGems(int x1, int y1, int x2, int y2);

    std::vector<std::pair<int, int>> getMatches() const;

    void removeMatches(const std::vector<std::pair<int, int>>& matches);

    void applyGravity();
    void refillFromTop();

    bool cascade();

    int getScore() const;

    void processBonusesForDestroyed(
        const std::vector<std::pair<int, int>>& destroyedPositions,
        const std::vector<Color>& originalColors
    );

    void applyRecolorBonus(int targetX, int targetY, Color sourceColor);
    void applyBombBonus(int bombX, int bombY);

private:
    int score;

    int width;
    int height;

    // grid[y][x]
    std::vector<std::vector<Color>> grid;

    std::mt19937 rng;

    void initRandomNoMatches();
    bool isInside(int x, int y) const;

    Color getRandomGemColor();
};