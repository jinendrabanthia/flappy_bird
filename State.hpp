#pragma once
#include <SFML/Graphics.hpp>

class Game; // Forward declaration

class State {
public:
    virtual ~State() = default;

    virtual void init() = 0;
    virtual void handleInput() = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderTarget& target, float alpha) = 0; // alpha for rendering interpolation
    virtual void pause() {}
    virtual void resume() {}
};
