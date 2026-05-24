#include "Board.h"

#include <set>
#include <algorithm>
#include <chrono>
#include <cmath>

Board::Board(int width, int height)
    : score(0),
    width(width),
    height(height),
    grid(height, std::vector<Color>(width, Color::Empty)),
    rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count())) {

    initRandomNoMatches();
}

bool Board::isValidPosition(int x, int y) const {
    return isInside(x, y);
}

int Board::getWidth() const {
    return width;
}

int Board::getHeight() const {
    return height;
}

int Board::getScore() const {
    return score;
}

bool Board::isInside(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

Color Board::getGem(int x, int y) const {
    if (!isInside(x, y)) {
        return Color::Empty;
    }

    return grid[y][x];
}

void Board::setGem(int x, int y, Color color) {
    if (!isInside(x, y)) {
        return;
    }

    grid[y][x] = color;
}

Color Board::getRandomGemColor() {
    std::uniform_int_distribution<int> dist(
        0,
        static_cast<int>(Color::Empty) - 1
    );

    return static_cast<Color>(dist(rng));
}

void Board::initRandomNoMatches() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            grid[y][x] = getRandomGemColor();
        }
    }

    while (true) {
        auto matches = getMatches();

        if (matches.empty()) {
            break;
        }

        for (const auto& match : matches) {
            int x = match.first;
            int y = match.second;

            Color current = grid[y][x];
            Color newColor = current;

            while (newColor == current) {
                newColor = getRandomGemColor();
            }

            grid[y][x] = newColor;
        }
    }
}

void Board::swapGems(int x1, int y1, int x2, int y2) {
    if (!isInside(x1, y1) || !isInside(x2, y2)) {
        return;
    }

    std::swap(grid[y1][x1], grid[y2][x2]);
}

std::vector<std::pair<int, int>> Board::getMatches() const {
    std::set<std::pair<int, int>> uniqueMatches;

    // Горизонтальные матчи
    for (int y = 0; y < height; ++y) {
        int start = 0;

        while (start < width) {
            Color current = grid[y][start];

            if (current == Color::Empty) {
                ++start;
                continue;
            }

            int end = start + 1;

            while (end < width && grid[y][end] == current) {
                ++end;
            }

            int length = end - start;

            if (length >= 3) {
                for (int x = start; x < end; ++x) {
                    uniqueMatches.insert({ x, y });
                }
            }

            start = end;
        }
    }

    // Вертикальные матчи
    for (int x = 0; x < width; ++x) {
        int start = 0;

        while (start < height) {
            Color current = grid[start][x];

            if (current == Color::Empty) {
                ++start;
                continue;
            }

            int end = start + 1;

            while (end < height && grid[end][x] == current) {
                ++end;
            }

            int length = end - start;

            if (length >= 3) {
                for (int y = start; y < end; ++y) {
                    uniqueMatches.insert({ x, y });
                }
            }

            start = end;
        }
    }

    return std::vector<std::pair<int, int>>(
        uniqueMatches.begin(),
        uniqueMatches.end()
    );
}

void Board::removeMatches(
    const std::vector<std::pair<int, int>>& matches) {

    for (const auto& match : matches) {
        int x = match.first;
        int y = match.second;

        setGem(x, y, Color::Empty);
    }
}

void Board::applyGravity() {
    for (int x = 0; x < width; ++x) {
        std::vector<Color> nonEmpty;

        for (int y = height - 1; y >= 0; --y) {
            if (grid[y][x] != Color::Empty) {
                nonEmpty.push_back(grid[y][x]);
            }
        }

        for (int y = 0; y < height; ++y) {
            grid[y][x] = Color::Empty;
        }

        int writeY = height - 1;

        for (Color color : nonEmpty) {
            grid[writeY][x] = color;
            --writeY;
        }
    }
}

void Board::refillFromTop() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] == Color::Empty) {
                grid[y][x] = getRandomGemColor();
            }
        }
    }
}

bool Board::cascade() {
    bool changed = false;

    while (true) {
        auto matches = getMatches();

        if (matches.empty()) {
            break;
        }

        changed = true;

        std::vector<Color> originalColors;
        originalColors.reserve(matches.size());

        for (const auto& pos : matches) {
            originalColors.push_back(getGem(pos.first, pos.second));
        }

        score += static_cast<int>(matches.size()) * 10;

        removeMatches(matches);

        processBonusesForDestroyed(matches, originalColors);

        applyGravity();
        refillFromTop();
    }

    return changed;
}

void Board::processBonusesForDestroyed(
    const std::vector<std::pair<int, int>>& destroyedPositions,
    const std::vector<Color>& originalColors) {

    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> bonusTypeDist(0, 1);

    for (size_t i = 0; i < destroyedPositions.size(); ++i) {
        if (chanceDist(rng) > 0.2f) {
            continue;
        }

        int centerX = destroyedPositions[i].first;
        int centerY = destroyedPositions[i].second;

        std::vector<std::pair<int, int>> nearbyCells;

        for (int dy = -3; dy <= 3; ++dy) {
            for (int dx = -3; dx <= 3; ++dx) {
                int nx = centerX + dx;
                int ny = centerY + dy;

                if (!isInside(nx, ny)) {
                    continue;
                }

                if (nx == centerX && ny == centerY) {
                    continue;
                }

                if (getGem(nx, ny) == Color::Empty) {
                    continue;
                }

                nearbyCells.push_back({ nx, ny });
            }
        }

        if (nearbyCells.empty()) {
            continue;
        }

        std::uniform_int_distribution<int> targetDist(
            0,
            static_cast<int>(nearbyCells.size()) - 1
        );

        auto target = nearbyCells[targetDist(rng)];

        int tx = target.first;
        int ty = target.second;

        int bonusType = bonusTypeDist(rng);

        if (bonusType == 0) {
            applyRecolorBonus(tx, ty, originalColors[i]);
        }
        else {
            applyBombBonus(tx, ty);
        }
    }
}

void Board::applyRecolorBonus(
    int targetX,
    int targetY,
    Color sourceColor) {

    if (!isInside(targetX, targetY)) {
        return;
    }

    setGem(targetX, targetY, sourceColor);

    std::vector<std::pair<int, int>> possibleCells;

    for (int dy = -3; dy <= 3; ++dy) {
        for (int dx = -3; dx <= 3; ++dx) {
            int nx = targetX + dx;
            int ny = targetY + dy;

            if (!isInside(nx, ny)) {
                continue;
            }

            if (nx == targetX && ny == targetY) {
                continue;
            }

            int manhattanDistance =
                std::abs(nx - targetX) + std::abs(ny - targetY);

            if (manhattanDistance == 1) {
                continue;
            }

            possibleCells.push_back({ nx, ny });
        }
    }

    std::shuffle(possibleCells.begin(), possibleCells.end(), rng);

    int recolorCount = std::min(2, static_cast<int>(possibleCells.size()));

    for (int i = 0; i < recolorCount; ++i) {
        int x = possibleCells[i].first;
        int y = possibleCells[i].second;

        setGem(x, y, sourceColor);
    }
}

void Board::applyBombBonus(int bombX, int bombY) {
    std::vector<std::pair<int, int>> allPositions;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            allPositions.push_back({ x, y });
        }
    }

    std::shuffle(allPositions.begin(), allPositions.end(), rng);

    bool bombIncluded = false;

    for (int i = 0; i < 5 && i < static_cast<int>(allPositions.size()); ++i) {
        int x = allPositions[i].first;
        int y = allPositions[i].second;

        setGem(x, y, Color::Empty);

        if (x == bombX && y == bombY) {
            bombIncluded = true;
        }
    }

    if (!bombIncluded && isInside(bombX, bombY)) {
        setGem(bombX, bombY, Color::Empty);
    }
}