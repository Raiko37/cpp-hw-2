#include "Game.h"

#include <iostream>
#include <exception>

int main() {
    try {
        const int WIDTH = 8;
        const int HEIGHT = 8;
        const int CELL_SIZE = 60;

        Game game(WIDTH, HEIGHT, CELL_SIZE);

        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}