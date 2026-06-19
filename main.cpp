#include "Game.hpp"
#include "GameState.hpp"
#include <memory>

int main() {
    Game game(800, 600, "Flappy Bird");
    game.pushState(std::make_unique<GameState>(game));
    game.run();
    return 0;
}
