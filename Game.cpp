#include "Game.h"

#include <cmath>

Game::Game(int boardWidth, int boardHeight, int cellSize)
    : board(boardWidth, boardHeight),
    window(
        sf::VideoMode(
            boardWidth* cellSize,
            boardHeight* cellSize
        ),
        "GEMS"
    ),
    renderer(window, cellSize),
    firstSelected(false),
    selectedX(0),
    selectedY(0) {
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            handleEvent(event);
        }

        window.clear(sf::Color::Black);

        renderer.drawBoard(board);

        renderer.drawScore(board.getScore());

        if (firstSelected) {
            renderer.drawSelection(selectedX, selectedY);
        }

        window.display();
    }
}

void Game::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        window.close();
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        int x = event.mouseButton.x / renderer.getCellSize();
        int y = event.mouseButton.y / renderer.getCellSize();

        if (!board.isValidPosition(x, y)) {
            return;
        }

        if (!firstSelected) {
            firstSelected = true;
            selectedX = x;
            selectedY = y;
            return;
        }

        // Клик по той же клетке
        if (selectedX == x && selectedY == y) {
            firstSelected = false;
            return;
        }

        int distance = std::abs(selectedX - x) +
            std::abs(selectedY - y);

        if (distance == 1) {
            trySwapAndMatch(selectedX, selectedY, x, y);
        }

        firstSelected = false;
    }
}

bool Game::trySwapAndMatch(int x1, int y1, int x2, int y2) {
    board.swapGems(x1, y1, x2, y2);

    auto matches = board.getMatches();

    if (!matches.empty()) {
        board.cascade();
        return true;
    }

    board.swapGems(x1, y1, x2, y2);

    return false;
}