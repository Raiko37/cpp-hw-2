#pragma once

#include <SFML/Graphics.hpp>
#include <map>

#include "Board.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& window, int cellSize);

    void drawBoard(const Board& board);
    void drawScore(int score);
    void drawSelection(int x, int y);

    int getCellSize() const {
        return cellSize;
    }

private:
    sf::RenderWindow& window;
    int cellSize;

    std::map<Color, sf::Color> colorMap;

    sf::Font font;

    sf::Color toSFMLColor(Color c) const;
};