#include "Renderer.h"
#include <string>

Renderer::Renderer(sf::RenderWindow& window, int cellSize)
    : window(window),
    cellSize(cellSize) {

    colorMap[Color::Red] = sf::Color::Red;
    colorMap[Color::Green] = sf::Color::Green;
    colorMap[Color::Blue] = sf::Color::Blue;
    colorMap[Color::Yellow] = sf::Color::Yellow;
    colorMap[Color::Purple] = sf::Color(128, 0, 128);
    colorMap[Color::Orange] = sf::Color(255, 165, 0);
    colorMap[Color::Empty] = sf::Color(50, 50, 50);

    
    font.loadFromFile("Roboto-Regular.ttf");
}

sf::Color Renderer::toSFMLColor(Color c) const {
    auto it = colorMap.find(c);

    if (it != colorMap.end()) {
        return it->second;
    }

    return sf::Color::White;
}

void Renderer::drawBoard(const Board& board) {
    for (int y = 0; y < board.getHeight(); ++y) {
        for (int x = 0; x < board.getWidth(); ++x) {
            sf::RectangleShape rect(
                sf::Vector2f(
                    static_cast<float>(cellSize - 1),
                    static_cast<float>(cellSize - 1)
                )
            );

            rect.setPosition(
                static_cast<float>(x * cellSize),
                static_cast<float>(y * cellSize)
            );

            rect.setFillColor(toSFMLColor(board.getGem(x, y)));

            rect.setOutlineThickness(1.0f);
            rect.setOutlineColor(sf::Color::Black);

            window.draw(rect);
        }
    }
}

void Renderer::drawScore(int score) {
    if (!font.getInfo().family.empty()) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setString("Score: " + std::to_string(score));
        text.setPosition(10.0f, 10.0f);

        window.draw(text);
    }
}

void Renderer::drawSelection(int x, int y) {
    sf::RectangleShape selection(
        sf::Vector2f(
            static_cast<float>(cellSize - 2),
            static_cast<float>(cellSize - 2)
        )
    );

    selection.setPosition(
        static_cast<float>(x * cellSize + 1),
        static_cast<float>(y * cellSize + 1)
    );

    selection.setFillColor(sf::Color::Transparent);

    selection.setOutlineThickness(2.0f);
    selection.setOutlineColor(sf::Color::Yellow);

    window.draw(selection);
}