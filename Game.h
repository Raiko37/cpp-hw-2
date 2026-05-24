#pragma once

#include <SFML/Graphics.hpp>

#include "Board.h"
#include "Renderer.h"

class Game {
public:
    Game(int boardWidth, int boardHeight, int cellSize);

    void run();

private:
    Board board;

    sf::RenderWindow window;

    Renderer renderer;

    bool firstSelected;

    int selectedX;
    int selectedY;

    void handleEvent(const sf::Event& event);

    bool trySwapAndMatch(int x1, int y1, int x2, int y2);
};