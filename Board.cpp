#include "Board.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <queue>

Board::Board(int width, int height)
    : score(0),
    width(width),
    height(height),
    grid(height),
    rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count())) {

    for (auto& row : grid) {
        row.resize(width);
    }

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
    if (!isInside(x, y) || !grid[y][x]) {
        return Color::Empty;
    }

    return grid[y][x]->getColor();
}

void Board::setGem(int x, int y, Color color) {
    if (!isInside(x, y)) {
        return;
    }

    if (color == Color::Empty) {
        grid[y][x].reset();
        return;
    }

    grid[y][x] = std::make_unique<StandardGem>(color);
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
            std::vector<Color> colors;

            for (int i = 0; i < static_cast<int>(Color::Empty); ++i) {
                colors.push_back(static_cast<Color>(i));
            }

            std::shuffle(colors.begin(), colors.end(), rng);

            bool placed = false;

            for (Color color : colors) {
                setGem(x, y, color);

                if (countConnectedSameColor(x, y) < 3) {
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                setGem(x, y, getRandomGemColor());
            }
        }
    }
}

void Board::swapGems(int x1, int y1, int x2, int y2) {
    if (!isInside(x1, y1) || !isInside(x2, y2)) {
        return;
    }

    grid[y1][x1].swap(grid[y2][x2]);
}

int Board::countConnectedSameColor(int startX, int startY) const {
    Color color = getGem(startX, startY);

    if (color == Color::Empty) {
        return 0;
    }

    std::vector<std::vector<bool>> visited(
        height,
        std::vector<bool>(width, false)
    );

    std::queue<std::pair<int, int>> queue;
    queue.push({ startX, startY });
    visited[startY][startX] = true;

    int count = 0;

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        ++count;

        for (int i = 0; i < 4; ++i) {
            int nx = current.first + dx[i];
            int ny = current.second + dy[i];

            if (!isInside(nx, ny) || visited[ny][nx]) {
                continue;
            }

            if (getGem(nx, ny) != color) {
                continue;
            }

            visited[ny][nx] = true;
            queue.push({ nx, ny });
        }
    }

    return count;
}

std::vector<std::pair<int, int>> Board::getMatches() const {
    std::vector<std::pair<int, int>> matches;

    std::vector<std::vector<bool>> visited(
        height,
        std::vector<bool>(width, false)
    );

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (visited[y][x]) {
                continue;
            }

            Color color = getGem(x, y);

            if (color == Color::Empty) {
                visited[y][x] = true;
                continue;
            }

            std::vector<std::pair<int, int>> component;
            std::queue<std::pair<int, int>> queue;

            queue.push({ x, y });
            visited[y][x] = true;

            while (!queue.empty()) {
                auto current = queue.front();
                queue.pop();

                component.push_back(current);

                for (int i = 0; i < 4; ++i) {
                    int nx = current.first + dx[i];
                    int ny = current.second + dy[i];

                    if (!isInside(nx, ny) || visited[ny][nx]) {
                        continue;
                    }

                    if (getGem(nx, ny) != color) {
                        continue;
                    }

                    visited[ny][nx] = true;
                    queue.push({ nx, ny });
                }
            }

            if (component.size() >= 3) {
                matches.insert(
                    matches.end(),
                    component.begin(),
                    component.end()
                );
            }
        }
    }

    return matches;
}

void Board::removeMatches(
    const std::vector<std::pair<int, int>>& matches) {

    for (const auto& match : matches) {
        setGem(match.first, match.second, Color::Empty);
    }
}

void Board::applyGravity() {
    for (int x = 0; x < width; ++x) {
        std::vector<std::unique_ptr<Gem>> nonEmpty;

        for (int y = height - 1; y >= 0; --y) {
            if (getGem(x, y) != Color::Empty) {
                nonEmpty.push_back(std::move(grid[y][x]));
            }
        }

        for (int y = 0; y < height; ++y) {
            grid[y][x].reset();
        }

        int writeY = height - 1;

        for (auto& gem : nonEmpty) {
            grid[writeY][x] = std::move(gem);
            --writeY;
        }
    }
}

void Board::refillFromTop() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (getGem(x, y) == Color::Empty) {
                setGem(x, y, getRandomGemColor());
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

    size_t count = std::min(destroyedPositions.size(), originalColors.size());

    for (size_t i = 0; i < count; ++i) {
        if (originalColors[i] == Color::Empty || chanceDist(rng) > 0.2f) {
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

        std::unique_ptr<Gem> bonus;

        if (bonusTypeDist(rng) == 0) {
            bonus = std::make_unique<RecolorBonusGem>(originalColors[i]);
        }
        else {
            bonus = std::make_unique<BombBonusGem>(originalColors[i]);
        }

        grid[ty][tx] = std::move(bonus);

        std::unique_ptr<Gem> activeBonus = std::move(grid[ty][tx]);

        activeBonus->activate(*this, tx, ty);
    }
}

void Board::applyRecolorBonus(
    int targetX,
    int targetY,
    Color sourceColor) {

    if (!isInside(targetX, targetY) || sourceColor == Color::Empty) {
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

            if (getGem(nx, ny) == Color::Empty) {
                continue;
            }

            possibleCells.push_back({ nx, ny });
        }
    }

    std::shuffle(possibleCells.begin(), possibleCells.end(), rng);

    int recolorCount = std::min(2, static_cast<int>(possibleCells.size()));

    for (int i = 0; i < recolorCount; ++i) {
        setGem(possibleCells[i].first, possibleCells[i].second, sourceColor);
    }
}

void Board::applyBombBonus(int bombX, int bombY) {
    std::vector<std::pair<int, int>> positions;

    if (isInside(bombX, bombY)) {
        positions.push_back({ bombX, bombY });
    }

    std::vector<std::pair<int, int>> candidates;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x == bombX && y == bombY) {
                continue;
            }

            if (getGem(x, y) == Color::Empty) {
                continue;
            }

            candidates.push_back({ x, y });
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);

    int needCount = std::min(
        5 - static_cast<int>(positions.size()),
        static_cast<int>(candidates.size())
    );

    for (int i = 0; i < needCount; ++i) {
        positions.push_back(candidates[i]);
    }

    for (const auto& pos : positions) {
        setGem(pos.first, pos.second, Color::Empty);
    }
}